#include <iostream>

#include "data.hpp"
#include "messages/list.hpp"
#include "messages/error.hpp"
#include "util.hpp"
#include "parser.hpp"
#include "instructions/signature.hpp"
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
            offset += chunk->bytes;
        }

        // Check if any labels left...
        for (auto chunk : data.chunks) {
            if (!chunk->is_data) {
                auto instruction = chunk->get_instruction();

                for (auto & arg : instruction->args) {
                    if (arg.is_label()) {
                        auto line = data.lines[chunk->source_line];

                        auto err = new class message::Error(data.file_path, line.n, 0, message::ErrorType::UnknownLabel);
                        err->set_message("Unresolved label reference '" + *arg.get_label() + "'");
                        msgs.add(err);
                        return;
                    }
                }
            }
        }
    }

    void parse_arg(const Data &data, int line_idx, int &col, message::List &msgs, instruction::Argument &argument) {
        auto line = data.lines[line_idx];

        if (line.data[col] == '\'') { // Character Literal
            col++;

            long long value;

            // Escape character
            if (line.data[col] == '\\') {
                value = decode_escape_seq(line.data, ++col);

                if (value == -1) {
                    auto err = new class message::Error(data.file_path, line.n, col, message::ErrorType::Syntax);
                    err->set_message("Invalid escape sequence");
                    msgs.add(err);
                    return;
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

            // Update argument data
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
        j = start;
        int radix = get_radix(sub.back());
        bool is_decimal = scan_number(line.data, radix, j);

        // So, do we have a literal?
        if (j > start) {
            col = j;

            // Default to base-10
            if (radix == -1) radix = 10;

            unsigned long long literal;
            j = start;

            // Set argument value
            if (is_decimal) {
                auto value = float_base_to_10(line.data, radix, j);
                literal = *(unsigned long long *) &value;
            } else {
                literal = int_base_to_10(line.data, radix, j);
            }

            argument.update(instruction::ArgumentType::Literal, literal);
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
}
