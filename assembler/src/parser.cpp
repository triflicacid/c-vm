#include <iostream>
#include <functional>

#include "parser.hpp"
#include "messages/error.hpp"
#include "util.hpp"
extern "C" {
#include "processor/src/registers.h"
}

namespace assembler::parser {
    void parse(Data &data, message::List &msgs) {
        // Track byte offset
        int offset = 0;

        for (int line_idx = 0; line_idx < data.lines.size(); line_idx++) {
            const auto &line = data.lines[line_idx];

            // Extract first item
            int start = 0, i = 0;
            skip_to_break(line.data, i);

            // Do we have a label?
            if (line.data[i - 1] == ':') {
                std::string label_name = line.data.substr(start, i - 1);

                // Check if valid label name
                if (!is_valid_label_name(label_name)) {
                    auto *err = new class message::Error(data.file_path, line.n, start, message::ErrorType::InvalidLabel);
                    err->set_message("Invalid label: '" + label_name + "'");
                    msgs.add(err);
                    return;
                }

                if (data.debug)
                    std::cout << "[" << line_idx << ":0] LABEL DECLARATION: " << label_name << "=" << offset << "\n";

                auto label = data.labels.find(label_name);

                if (label == data.labels.end()) {
                    // Create a new label
                    data.labels.insert({label_name, {line.n, start, offset } });
                } else {
                    // Warn user that the label already exists (error if main)
                    auto level = label_name == data.main_label ? message::Level::Error : message::Level::Warning;
                    auto *msg = new message::Message(level, data.file_path, line.n, start);
                    msg->set_message("Re-declaration of label " + label_name);
                    msgs.add(msg);

                    msg = new message::Message(message::Level::Note, data.file_path, label->second.line, label->second.col);
                    msg->set_message("Previously declared here");
                    msgs.add(msg);

                    // Exit if error
                    if (level == message::Level::Error)
                        return;

                    // Update label's information
                    label->second.line = line.n;
                    label->second.col = start;
                    label->second.addr = offset;
                }

                // Replace all past references with its address
                data.replace_label(label_name, offset);

                // End of input?
                if (i == line.data.size()) {
                    continue;
                } else {
                    skip_whitespace(line.data, i);
                    start = i;
                    skip_to_break(line.data, i);
                }
            }

            // Interpret as an instruction mnemonic
            std::string mnemonic = line.data.substr(start, i - start);

            if (data.debug)
                std::cout << "[" << line_idx << ":" << start << "] Mnemonic " << mnemonic << "\n";

            // Is constant specifier?
            std::vector<unsigned char> *bytes = nullptr;

            if (parse_data(data, line_idx, start, msgs, &bytes)) {
                if (data.debug)
                    std::cout << "\tData (" << bytes->size() << " bytes)\n";

                // Check for errors
                if (msgs.has_message_of(message::Level::Error)) {
                    delete bytes;
                    return;
                }

                // Insert into a Chunk
                auto chunk = new Chunk(line_idx, offset);
                chunk->set_data(bytes);
                offset += chunk->get_bytes();
                data.chunks.push_back(chunk);

                continue;
            }

            // Before anything, check if mnemonic exists
            if (!instruction::Signature::exists(mnemonic)) {
                auto err = new class message::Error(data.file_path, line.n, start, message::ErrorType::UnknownMnemonic);
                err->set_message("Unknown mnemonic '" + mnemonic + "'");
                msgs.add(err);
                return;
            }

            // Parse arguments
            std::vector<instruction::Argument> arguments;
            std::vector<instruction::ArgumentType> argument_types;

            while (i < line.data.size()) {
                skip_whitespace(line.data, i);

                // Parse argument
                instruction::Argument argument;
                parse_arg(data, line_idx, i, msgs, argument);

                // Must end in break character
                if (i < line.data.size() && line.data[i] != ' ' && line.data[i] != ',') {
                    std::string ch(1, line.data[i]);

                    auto err = new class message::Error(data.file_path, line.n, i, message::ErrorType::Syntax);
                    err->set_message("Expected ' ' or ',', got '" + ch + "'");
                    msgs.add(err);
                }

                // Tell user which argument it was
                if (msgs.has_message_of(message::Level::Error)) {
                    auto msg = new message::Message(message::Level::Note, data.file_path, line.n, 0);
                    msg->set_message("While parsing mnemonic " + mnemonic + " argument #" + std::to_string(arguments.size() + 1));
                    msgs.add(msg);

                    return;
                }

                // Add to argument list
                arguments.push_back(argument);
                argument_types.push_back(argument.get_type());

                if (data.debug) {
                    std::cout << "\tArg: ";
                    argument.print();
                    std::cout << "\n";
                }

                // Skip next
                if (line.data[i] == ',')
                    i++;
            }

            // Search for operation
            auto signature = instruction::Signature::find(mnemonic, argument_types);

            if (signature == nullptr) {
                // Build argument string
                std::stringstream stream;
                stream << "Unknown arguments for mnemonic " << mnemonic << ": ";

                for (int j = 0; j < arguments.size(); j++) {
                    arguments[j].print(stream);

                    if (j < arguments.size() - 1)
                        stream << ", ";
                }

                auto err = new class message::Error(data.file_path, line.n, start, message::ErrorType::BadArguments);
                err->set_message(stream);
                msgs.add(err);
                return;
            }

            // Build instruction
            auto instruction = new instruction::Instruction(*signature, arguments);

            // Insert into a Chunk
            auto chunk = new Chunk(line_idx, offset);
            chunk->set_instruction(instruction);

            data.chunks.push_back(chunk);
            offset += chunk->get_bytes();
        }

        // Check if any labels left...
        for (auto chunk : data.chunks) {
            if (!chunk->is_data()) {
                auto instruction = chunk->get_instruction();

                for (auto & arg : instruction->args) {
                    if (arg.is_label()) {
                        auto line = data.lines[chunk->get_source_line()];

                        auto err = new class message::Error(data.file_path, line.n, 0, message::ErrorType::UnknownLabel);
                        err->set_message("Unresolved label reference '" + *arg.get_label() + "'");
                        msgs.add(err);
                        return;
                    }
                }
            }
        }
    }

    /** Add byte sequence to new vector, cast all to integers (type #1). */
    template<typename T>
    std::vector<unsigned char>* add_byte_sequence(const Data &data, int line_idx, int &col, message::List &msgs) {
        auto bytes = new std::vector<unsigned char>();

        parse_byte_sequence(data, line_idx, col, msgs, [bytes](long long v_int, double v_dbl, bool is_dbl) {
            T value = static_cast<T>(v_int);

            for (int i = 0; i < sizeof(T); i++) {
                bytes->push_back((value >> (i * 8)) & 0xFF);
            }
        });

        // If empty, add 0
        if (bytes->empty()) {
            for (int i = 0; i < sizeof(T); i++) {
                bytes->push_back(0);
            }
        }

        return bytes;
    }

    /** Add byte sequence to new vector, cast all to floats (type #1), store an type #2 (int type of same size). */
    template<typename T, typename S>
    std::vector<unsigned char>* add_byte_sequence_float(const Data &data, int line_idx, int &col, message::List &msgs) {
        auto bytes = new std::vector<unsigned char>();

        parse_byte_sequence(data, line_idx, col, msgs, [bytes](long long v_int, double v_dbl, bool is_dbl) {
            T intermediate = static_cast<T>(is_dbl ? v_dbl : v_int);
            S value = *(S *) &intermediate;

            for (int i = 0; i < sizeof(S); i++) {
                bytes->push_back((value >> (i * 8)) & 0xFF);
            }
        });

        // If empty, add 0
        if (bytes->empty()) {
            for (int i = 0; i < sizeof(S); i++) {
                bytes->push_back(0);
            }
        }

        return bytes;
    }

    bool parse_data(const Data &data, int line_idx, int &col, message::List &msgs, std::vector<unsigned char> **bytes) {
        auto& line = data.lines[line_idx];

        // Extract datatype
        int start = col;
        skip_non_whitespace(line.data, col);
        std::string datatype = line.data.substr(start, col - start);

        std::function<void(unsigned char)> add_byte = [bytes](unsigned char byte) {
            (*bytes)->push_back(byte);
        };

        if (datatype == "u8") {
            *bytes = add_byte_sequence<uint8_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "i8") {
            *bytes = add_byte_sequence<int8_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "u16") {
            *bytes = add_byte_sequence<uint16_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "i16") {
            *bytes = add_byte_sequence<int16_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "u32") {
            *bytes = add_byte_sequence<uint32_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "i32") {
            *bytes = add_byte_sequence<int32_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "u64") {
            *bytes = add_byte_sequence<uint64_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "i64") {
            *bytes = add_byte_sequence<int64_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "f32") {
            *bytes = add_byte_sequence_float<float, uint32_t>(data, line_idx, col, msgs);
            return true;
        }

        if (datatype == "f64") {
            *bytes = add_byte_sequence_float<double, uint64_t>(data, line_idx, col, msgs);
            return true;
        }

        col = start;
        return false;
    }

    void parse_arg(const Data &data, int line_idx, int &col, message::List &msgs, instruction::Argument &argument) {
        auto line = data.lines[line_idx];

        if (line.data[col] == '\'') { // Character Literal
            unsigned long long value;
            parse_character_literal(data, line_idx, ++col, msgs, value);

            // Any errors?
            if (msgs.has_message_of(message::Level::Error)) {
                return;
            }

            // Update argument
            argument.update(instruction::ArgumentType::Literal, value);
            return;
        }

        if (line.data[col] == '[') { // Address, register pointer, label (addr)
            int start = ++col;
            parse_arg_lit(data, line_idx, col, msgs, argument);

            if (msgs.has_message_of(message::Level::Error))
                return;

            // Check if something was parsed
            if (start < col) {
                // Must end with a closing bracket
                if (line.data[col] != ']') {
                    std::string ch(1, line.data[col]);

                    message::Message *msg = new class message::Error(data.file_path, line.n, col, message::ErrorType::Syntax);
                    msg->set_message("Expected ']', got '" + ch + "'");
                    msgs.add(msg);

                    msg = new message::Message(message::Level::Note, data.file_path, line.n, start - 1);
                    msg->set_message("Group opened here");
                    msgs.add(msg);

                    return;
                }

                col++;

                // Transform argument datatype
                argument.transform_address_equivalent();

                return;
            }
        }

        if (line.data[col] == '+' || line.data[col] == '-' || std::isalnum(line.data[col])) { // Numeric literal, register, label (lit)
            int start = col;
            parse_arg_lit(data, line_idx, col, msgs, argument);

            if (msgs.has_message_of(message::Level::Error))
                return;

            // Check if something was parsed
            if (start < col) {
                return;
            }
        }

        // Assume error
        std::string ch(1, line.data[col]);

        auto err = new class message::Error(data.file_path, line.n, col, message::ErrorType::Syntax);
        err->set_message("Unexpected character '" + ch + "'");
        msgs.add(err);
    }

    void parse_arg_lit(const Data &data, int line_idx, int &col, message::List &msgs, instruction::Argument &argument) {
        auto& line = data.lines[line_idx];

        // Extract characters
        int start = col;
        skip_to_break(line.data,col);
        std::string sub = line.data.substr(start, col - start);

        // Is register?
        int j = start;
        int reg_offset = parse_register(line.data, j);

        if (reg_offset != -1) {
            col = j;
            argument.update(instruction::ArgumentType::Register, reg_offset);
            return;
        }

        // Parse as number
        unsigned long long number;
        double _1;
        bool _2;

        if (parse_number(sub, _2, number, _1)) {
            argument.update(instruction::ArgumentType::Literal, number);
            return;
        }

        // Must be a label, then
        auto label = data.labels.find(sub);

        // New label?
        if (label == data.labels.end()) {
            // Check if name is valid - if not, report generic syntax error (not label error)
            if (!is_valid_label_name(sub)) {
                auto err = new class message::Error(data.file_path, line.n, col, message::ErrorType::Syntax);
                err->set_message("Syntax Error: '" + sub + "'");
                msgs.add(err);
                return;
            }

            argument.update(instruction::ArgumentType::LabelLiteral, 0);
            argument.set_label(sub);
        } else {
            // Substitute label value
            argument.transform_label(label->second.addr);
        }
    }

    int parse_register(const std::string& s, int &i) {
        if (s[i] == 'r' && std::isdigit(s[i + 1])) {
            i += 2;
            return s[i - 1] - '0';
        }

        if (s == REG_FLAG_SYM) {
            i += sizeof(REG_FLAG_SYM) - 1;
            return REG_FLAG;
        }

        if (s == REG_CMP_SYM) {
            i += sizeof(REG_CMP_SYM) - 1;
            return REG_CMP;
        }

        if (s == REG_CCR_SYM) {
            i += sizeof(REG_CCR_SYM) - 1;
            return REG_CCR;
        }

        if (s == REG_ERR_SYM) {
            i += sizeof(REG_ERR_SYM) - 1;
            return REG_ERR;
        }

        if (s == REG_IP_SYM) {
            i += sizeof(REG_IP_SYM) - 1;
            return REG_IP;
        }

        if (s == REG_SP_SYM) {
            i += sizeof(REG_SP_SYM) - 1;
            return REG_SP;
        }

        if (s == REG_STACK_SIZE_SYM) {
            i += sizeof(REG_STACK_SIZE_SYM) - 1;
            return REG_STACK_SIZE;
        }

        if (s == REG_FP_SYM) {
            i += sizeof(REG_FP_SYM) - 1;
            return REG_FP;
        }

        return -1;
    }

    bool parse_number(const std::string& string, bool& is_decimal, unsigned long long& v_int, double &v_dbl) {
        int i = 0;
        int radix = get_radix(string.back());
        is_decimal = scan_number(string, radix, i);

        // So, do we have a literal?
        if (i > 0) {
            i = 0;

            // Default to base-10
            if (radix == -1) radix = 10;

            // Set argument value
            if (is_decimal) {
                v_dbl = float_base_to_10(string, radix, i);
                v_int = *(unsigned long long *) &v_dbl;
            } else {
                v_int = int_base_to_10(string, radix, i);
                v_dbl = *(double *) &v_int;
            }

            return true;
        } else {
            return false;
        }
    }

    void parse_character_literal(const Data &data, int line_idx, int &col, message::List &msgs, unsigned long long& value) {
        auto& line = data.lines[line_idx];

        // Escape character
        if (line.data[col] == '\\') {
            auto raw_value = decode_escape_seq(line.data, ++col);

            if (value == -1) {
                auto err = new class message::Error(data.file_path, line.n, col, message::ErrorType::Syntax);
                err->set_message("Invalid escape sequence");
                msgs.add(err);
                return;
            } else {
                value = raw_value;
            }
        } else {
            value = (unsigned char) line.data[col++];
        }

        // Check for ending apostrophe
        if (line.data[col] != '\'') {
            auto err = new class message::Error(data.file_path, line.n, col, message::ErrorType::Syntax);
            err->set_message("Expected apostrophe to terminate character literal");
            msgs.add(err);
            return;
        }

        col++;
    }

    void parse_byte_item(const Data &data, int line_idx, int &col, message::List &msgs, const AddBytesFunction& add_bytes) {
        auto& line = data.lines[line_idx];

        // Have we a character?
        if (line.data[col] == '\'') {
            unsigned long long value;
            parse_character_literal(data, line_idx, ++col, msgs, value);

            if (msgs.has_message_of(message::Level::Error)) {
                return;
            }

            add_bytes(value, 0.0, false);
            return;
        }

        // Have we a string?
        if (line.data[col] == '"') {
            col++;

            while (col < line.data.size() && line.data[col] != '"') {
                // Escape character
                if (line.data[col] == '\\') {
                    auto value = decode_escape_seq(line.data, ++col);

                    if (value == -1) {
                        auto err = new class message::Error(data.file_path, line.n, col, message::ErrorType::Syntax);
                        err->set_message("Invalid escape sequence");
                        msgs.add(err);
                        return;
                    } else {
                        add_bytes(value, 0.0, false);
                    }
                } else {
                    add_bytes(line.data[col++], 0.0, false);
                }
            }

            // Must terminate string
            if (line.data[col] != '"') {
                auto err = new class message::Error(data.file_path, line.n, col, message::ErrorType::Syntax);
                err->set_message("Unterminated string literal");
                msgs.add(err);
                return;
            }

            col++;
            return;
        }

        // Extract characters
        int start = col;
        skip_to_break(line.data,col);
        std::string sub = line.data.substr(start, col - start);

        // Is register? Not allowed here.
        int j = start;
        int reg_offset = parse_register(line.data, j);

        if (reg_offset != -1) {
            auto err = new class message::Error(data.file_path, line.n, start, message::ErrorType::Syntax);
            err->set_message("Register symbol disallowed here '" + sub + "'");
            msgs.add(err);
            return;
        }

        // Parse as number
        unsigned long long number_int;
        double number_dbl;
        bool is_dbl;

        if (parse_number(sub, is_dbl, number_int, number_dbl)) {
            add_bytes(number_int, number_dbl, is_dbl);
            return;
        }

        // Must be a label, then
        auto label = data.labels.find(sub);

        // New label?
        if (label == data.labels.end()) {
            // TODO allow forward-referenced labels in data?

            message::Message *msg = new class message::Error(data.file_path, line.n, col, message::ErrorType::UnknownLabel);
            msg->set_message("Reference to undefined label '" + sub + "'");
            msgs.add(msg);

            msg = new message::Message(message::Level::Note, data.file_path, line.n, col);
            msg->set_message("Labels in data cannot be forward-referenced");
            msgs.add(msg);

            return;
        } else {
            add_bytes(label->second.addr, 0.0, false);
            return;
        }
    }

    void parse_byte_sequence(const Data &data, int line_idx, int &col, message::List &msgs, const AddBytesFunction& add_bytes) {
        auto& line = data.lines[line_idx];
        int start = col;

        while (true) {
            skip_whitespace(line.data, col);

            if (col == line.data.size())
                break;

            // Parse item
            parse_byte_item(data, line_idx, col, msgs, add_bytes);

            // Any errors?
            if (msgs.has_message_of(message::Level::Error)) {
                auto msg = new message::Message(message::Level::Note, data.file_path, line.n, start);
                msg->set_message("Data sequence starts here");
                msgs.add(msg);

                return;
            }

            // Skip comma
            if (line.data[col] == ',')
                col++;
        }
    }
}
