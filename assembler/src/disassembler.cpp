#include <iostream>
#include "disassembler.hpp"
#include "messages/error.hpp"
#include "instructions/signature.hpp"
extern "C" {
#include "processor/src/opcodes.h"
#include "util.h"
}

namespace disassembler {
    /** Disassembler given data. */
    void disassemble(Data &data, message::List &msgs) {
        int pos = 0;

        // Skip past starting position
        WORD_T start_addr = *(WORD_T *)(data.buffer + pos);
        pos += sizeof(start_addr);

        if (data.debug)
            std::cout << "Start address: +" << start_addr << "\n";

        std::vector<unsigned char> data_bytes;

        while (pos < data.buffer_size) {
            // Read current byte as opcode
            OPCODE_T opcode = *(OPCODE_T *)(data.buffer + pos);

            // Look-up opcode
            auto signature = assembler::instruction::Signature::find(opcode);

            // Unknown opcode; treat as data
            if (true || signature == nullptr) {
                for (int off = 0; off < sizeof(opcode); ++off) {
                    unsigned char byte = (opcode >> (off * 8)) & 0xFF;
                    data_bytes.push_back(byte);
                }

                // Advance position pointer
                pos += sizeof(opcode);

                continue;
            }

            // If we were writing data, end it
            if (!data_bytes.empty()) {
                if (data.debug)
                    std::cout << "Writing " << data_bytes.size() << " bytes of data.\n";

                write_data_to_stream(data.assembly, data_bytes, data.format_data);
                data_bytes.clear();
            }

            // Write opcode
            // TODO
        }

        // Ensure all bytes are written
        if (!data_bytes.empty()) {
            if (data.debug)
                std::cout << "Writing " << data_bytes.size() << " bytes of data.\n";

            write_data_to_stream(data.assembly, data_bytes, data.format_data);
        }
    }

    void write_data_to_stream(std::stringstream &stream, const std::vector<unsigned char> &bytes, bool format) {
        if (!bytes.empty()) {
            stream << "u8 ";

            if (format) {
                std::vector<unsigned char> printable_chars;

                for (int byte : bytes) {
                    // Printable character?
                    if (std::isalnum(byte) || (byte == 0 && !printable_chars.empty())) {
                        printable_chars.push_back(byte);
                        continue;
                    }

                    // Write printable characters?
                    if (!printable_chars.empty()) {
                        write_chars_to_stream(stream, printable_chars);
                        printable_chars.clear();
                        stream << " ";
                    }

                    if (byte < 10) {
                        // Print as decimal
                        stream << std::dec << byte << " ";
                    } else {
                        // Print as hex
                        stream << std::hex << std::uppercase << byte << "h ";
                    }
                }

                // Write left-over printable characters?
                write_chars_to_stream(stream, printable_chars);
            } else {
                stream << std::dec;

                for (int byte : bytes) {
                    stream << (int) byte << " ";
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
}
