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

        bool is_writing_data = false;

        while (pos < data.buffer_size) {
            // Read current byte as opcode
            OPCODE_T opcode = *(OPCODE_T *)(data.buffer + pos);

            // Look-up opcode
            auto signature = assembler::instruction::Signature::find(opcode);

            if (true || signature == nullptr) {
                // Interpret as data
                if (data.debug)
                    std::cout << "[+" << pos << "] Data: " << sizeof(opcode) << " byte(s).\n";

                // If not already, print data-type mnemonic
                if (!is_writing_data) {
                    data.assembly << "u8 ";
                    is_writing_data = true;
                }

                // Write bytes to assembly buffer
                for (int off = 0; off < sizeof(opcode); ++off) {
                    data.assembly << ((opcode >> (off * 8)) & 0xFF) << " ";
                }

                // Advance position pointer
                pos += sizeof(opcode);

                continue;
            }

            // If we were writing data, end it
            if (is_writing_data) {
                data.assembly << "\n";
                is_writing_data = false;
            }

            // Write opcode
            // TODO
        }
    }
}
