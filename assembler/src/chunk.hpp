#pragma once

#include <vector>
#include "instructions/instruction.hpp"

namespace assembler {
    class Chunk {
    public:
        bool is_data; // Is data or an instruction?
        int offset;  // Byte offset
        int bytes;   // Byte length
        void *data;  // Internal data
        int source_line; // Index of source line

        Chunk(int line_idx, int offset) {
            this->offset = offset;
            this->source_line = line_idx;
            is_data = false;
            bytes = 0;
            data = nullptr;
        }

        ~Chunk();

        void print() const;

        /** Interpret data as an instruction. */
        [[nodiscard]] instruction::Instruction *get_instruction() const { return (instruction::Instruction *) data; }

        /** Set Chunk to an instruction. */
        void set_instruction(instruction::Instruction *instruction);

        /** Return index of chunk in the given range, or -1. */
        static int get_in_range(const std::vector<Chunk>& chunks, int lower, int upper);
    };
}
