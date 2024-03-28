#include <iostream>
#include "disassembler.hpp"
#include "messages/error.hpp"
extern "C" {
#include "processor/src/registers.h"
#include "util.h"
}

namespace disassembler {
    /** Disassembler given data. */
    void disassemble(Data &data, message::List &msgs) {
        if (data.debug)
            std::cout << "Start address: +" << data.start_addr << "\n";

        // Track current position
        int pos = 0;

        std::vector<unsigned char> data_bytes;

        // Initial pass: populate map data structures
        while (pos < data.buffer_size) {
            // Read current byte as opcode
            OPCODE_T opcode = *(OPCODE_T *)(data.buffer + pos);

            // Look-up opcode
            auto signature = assembler::instruction::Signature::find(opcode);

            // Unknown opcode
            if (signature == nullptr) {
                for (int off = 0; off < sizeof(opcode); ++off) {
                    data_bytes.push_back((opcode >> (off * 8)) & 0xFF);
                }

                pos += sizeof(opcode);
                continue;
            }

            // Not enough space left for instruction
            if (pos + signature->get_bytes() >= data.buffer_size) {
                for (; pos < data.buffer_size; pos++) {
                    data_bytes.push_back(data.buffer[pos]);
                }

                break;
            }

            // Record data segment if necessary
            if (!data_bytes.empty()) {
                data.data_offsets.insert({ pos - data_bytes.size(), data_bytes });
                data_bytes.clear();
            }

            // Record instruction
            data.instruction_offsets.insert({ pos, signature });
            pos += signature->get_bytes();
        }

        // If more data in buffer, record it
        if (!data_bytes.empty()) {
            data.data_offsets.insert({ pos - data_bytes.size(), data_bytes });
            data_bytes.clear();
        }

        // Check if any data segments are referenced inside opcodes for data labels
        if (data.insert_labels) {
            int data_label_idx = 0;
            int pos_label_idx = 0;

            for (auto &pair: data.instruction_offsets) {
                pos = pair.first + (int) sizeof(pair.second->get_opcode());

                // Iterate over instruction arguments
                for (int i = 0; i < pair.second->param_count(); i++) {
                    auto param = pair.second->get_param(i);
                    bool is_lit_addr = param->type == assembler::instruction::ParamType::Literal ||
                                       param->type == assembler::instruction::ParamType::Address;

                    // Check if we have a JMP instruction
                    if (assembler::instruction::is_jmp_opcode(pair.second->get_opcode())) {
                        if (is_lit_addr) {
                            // Extract location
                            auto value = extract_number(data.buffer, param->size, pos);

                            data.pos_labels.insert({value, pos_label_idx++});
                        }
                    } else if (is_lit_addr) {
                        // Extract location
                        auto value = extract_number(data.buffer, param->size, pos);

                        // Is there a data segment at this location?
                        auto segment = data.data_offsets.find((int) value);

                        if (segment != data.data_offsets.end()) {
                            if (data.debug)
                                std::cout << "[+" << pos << "] Found literal/address pointing to data segment at +"
                                          << value << "\n";

                            data.data_labels.insert({value, data_label_idx++});
                        }
                    }

                    pos += param->size;
                }
            }
        }

        // Write assembly source
        pos = 0;

        while (pos <= data.buffer_size) {
            // Main label?
            if (pos > 0 && pos == data.start_addr) {
                data.assembly << data.main_label << ":\n";
            }

            // Position label?
            auto pos_label = data.pos_labels.find(pos);

            if (pos_label != data.pos_labels.end()) {
                data.assembly << data.get_pos_label(pos_label->second) << ":\n";
            }

            auto found_data = data.data_offsets.find(pos);

            // We have a data segment
            if (found_data != data.data_offsets.end()) {
                write_data_segment(data, pos);
                continue;
            }

            auto found_op = data.instruction_offsets.find(pos);

            // We have an instruction segment
            if (found_op != data.instruction_offsets.end()) {
                write_signature_to_stream(data, *found_op->second, pos);
                continue;
            }

            pos++;
        }
    }

    void write_data_segment(Data &data, int &offset) {
        // Find segment at offset
        auto segment = data.data_offsets.find(offset);

        if (segment == data.data_offsets.end())
            return;

        // Predicated by a label?
        auto label_num = data.data_labels.find(offset);

        if (label_num != data.data_labels.end()) {
            data.assembly << data.get_data_label(label_num->second) << ": ";
        }

        // Write data segment
        write_data_to_stream(data.assembly, segment->second, data.format_data, data.insert_commas);
        offset += (int) segment->second.size();
    }

    void write_data_to_stream(std::stringstream &stream, const std::vector<unsigned char> &bytes, bool format, bool commas) {
        if (!bytes.empty()) {
            stream << "u8 ";

            if (format) {
                std::vector<unsigned char> printable_chars;

                for (int i = 0; i < bytes.size(); i++) {
                    int byte = bytes[i];

                    // Printable character?
                    if (std::isalnum(byte) || std::isspace(byte) || (!printable_chars.empty() && (byte == 0 || (byte > 31 && byte < 127)))) {
                        printable_chars.push_back(byte);
                        continue;
                    }

                    // Write printable characters?
                    if (!printable_chars.empty()) {
                        write_chars_to_stream(stream, printable_chars);
                        printable_chars.clear();
                        stream << (commas ? ", " : " ");
                    }

                    if (byte < 10) {
                        // Print as decimal
                        stream << std::dec << byte;
                    } else {
                        // Print as hex
                        stream << std::hex << std::uppercase << byte << "h";
                    }

                    if (i < bytes.size() - 1)
                        stream << (commas ? ", " : " ");
                }

                // Write left-over printable characters?
                write_chars_to_stream(stream, printable_chars);
            } else {
                stream << std::dec;

                for (int i = 0; i < bytes.size(); i++) {
                    stream << (int) bytes[i];

                    if (i < bytes.size() - 1)
                        stream << (commas ? ", " : " ");
                }
            }

            stream << "\n";
        }
    }

    void write_chars_to_stream(std::stringstream &stream, const std::vector<unsigned char> &chars) {
        if (!chars.empty()) {
            // Decide literal delimiter
            char delim = chars.size() == 1 ? '\'' : '"';

            stream << delim;

            for (auto c : chars) {
                if (c == 0) {
                    stream << "\\0";
                } else if (c == '\n') {
                    stream << "\\n";
                } else if (c == '\r') {
                    stream << "\\r";
                } else if (c == delim) {
                    stream << "\\" << c;
                } else {
                    stream << c;
                }
            }

            stream << delim;
        }
    }

    void write_signature_to_stream(Data &data, assembler::instruction::Signature &signature, int &ptr) {
        // Write mnemonic
        data.assembly << signature.get_mnemonic() << " ";
        ptr += sizeof(signature.get_opcode());

        if (data.debug)
            std::cout << "[+" << ptr << "] Mnemonic " << signature.get_opcode() << ", " << signature.get_mnemonic()
                << ". Size: " << signature.get_bytes() << " bytes.\n";

        // Iterate over parameters
        int count = (int) signature.param_count();
        for (int i = 0; i < count; i++) {
            const auto param = signature.get_param(i);

            // Extract value
            unsigned long long value = extract_number(data.buffer, param->size, ptr);

            if (param->type == assembler::instruction::ParamType::Literal || param->type == assembler::instruction::ParamType::Address) {
                std::string label;

                // Is JMP?
                if (assembler::instruction::is_jmp_opcode(signature.get_opcode())) {
                    auto pos_label = data.pos_labels.find((int) value);

                    if (pos_label != data.pos_labels.end()) {
                        label = data.get_pos_label(pos_label->second);
                    }
                }

                // Points to data segment?
                if (label.empty()) {
                    auto data_label = data.data_labels.find((int) value);

                    if (data_label != data.data_labels.end()) {
                        label = data.get_data_label(data_label->second);
                    }
                }

                if (param->type == assembler::instruction::ParamType::Address) {
                    if (label.empty()) {
                        if (data.debug)
                            std::cout << "\tArg: address " << value << "\n";

                        data.assembly << "[" << value << "]";
                    } else {
                        if (data.debug)
                            std::cout << "\tArg: label (addr.) " << label << " (" << value << ")\n";

                        data.assembly << "[" << label << "]";
                    }
                } else {
                    if (label.empty()) {
                        if (data.debug)
                            std::cout << "\tArg: literal " << value << "\n";

                        data.assembly << value;
                    } else {
                        if (data.debug)
                            std::cout << "\tArg: label (lit.) " << label << " (" << value << ")\n";

                        data.assembly << label;
                    }
                }
            } else {
                // Register/Register pointer
                auto reg = register_to_string((int) value);

                if (param->type == assembler::instruction::ParamType::RegisterPointer) {
                    if (data.debug)
                        std::cout << "\tArg: register pointer " << value << " ([" << reg << "])\n";

                    data.assembly << "[" << reg << "]";
                } else {
                    if (data.debug)
                        std::cout << "\tArg: register " << value << " (" << reg << ")\n";

                    data.assembly << reg;
                }
            }

            if (i != count - 1)
                data.assembly << (data.insert_commas ? ", " : " ");

            ptr += param->size;
        }

        data.assembly << "\n";
    }

    std::string register_to_string(int reg) {
        switch (reg) {
            case REG_FLAG:
                return REG_FLAG_SYM;
//            case REG_CMP:
//                return REG_CMP_SYM;
            case REG_CCR:
                return REG_CCR_SYM;
            case REG_ERR:
                return REG_ERR_SYM;
            case REG_IP:
                return REG_IP_SYM;
            case REG_SP:
                return REG_SP_SYM;
            case REG_STACK_SIZE:
                return REG_STACK_SIZE_SYM;
            case REG_FP:
                return REG_FP_SYM;
            default:
                return "r" + std::to_string(reg);
        }
    }

    unsigned long long extract_number(const char *buffer, int size, int ptr) {
        switch (size) {
            case 1:
                return (int8_t) buffer[ptr];
            case 2:
                return *(int16_t *) (buffer + ptr);
            case 4:
                return *(int32_t *) (buffer + ptr);
            case 8:
                return *(int64_t *) (buffer + ptr);
            default:
                return 0;
        }
    }
}
