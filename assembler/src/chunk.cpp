#include "chunk.hpp"
extern "C" {
#include "util.h"
}

#include <iostream>
#include <vector>

namespace assembler {
    Chunk::~Chunk() {
        if (is_data) {
            free(data);
        } else {
            get_instruction()->~Instruction();
        }
    }

    void Chunk::print() const {
        std::cout << "Chunk at +" << offset << " of " << bytes << " bytes";

        if (is_data) {
            printf(" - data:\n\t{");
            std::cout << " - data:\n\t{";
            print_bytes(data, bytes);
            std::cout << "\b}\n";
        } else {
            std::cout << " - instruction:\n\t";
            get_instruction()->print();
        }
    }

    int Chunk::get_in_range(const std::vector<Chunk>& chunks, int lower, int upper) {
        int idx = 0;
        for (const Chunk& chunk : chunks) {
            if (chunk.offset <= upper && chunk.offset + chunk.bytes > lower) {
                return idx;
            }

            idx++;
        }

        return -1;
    }

    void Chunk::set_instruction(instruction::Instruction *instruction) {
        is_data = false;
        bytes = instruction->bytes;
        data = instruction;
    }
}
