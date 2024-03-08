#pragma once

#include <vector>

namespace assembler {
    class Chunk {
    public:
        bool is_data; // Is data or an instruction?
        int offset;  // Byte offset
        int bytes;   // Byte length
        void *data;  // Internal data
        int source_line; // Index of source line

        ~Chunk();

        void print() const;

        /** Return index of chunk in the given range, or -1. */
        static int get_in_range(const std::vector<Chunk>& chunks, int lower, int upper);
    };
}
