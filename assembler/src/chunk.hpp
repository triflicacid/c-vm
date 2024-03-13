#pragma once

#include "instructions/instruction.hpp"

namespace assembler {
    class Chunk {
    private:
        bool m_is_data; // Is data or an instruction?
        int m_offset;  // Byte offset
        int m_bytes;   // Byte length
        void *m_ptr;  // Internal data, either std::string* or std::vector<unsigned char>*
        int m_source_line; // Index of source line

        void free_ptr() const;

    public:
        Chunk(int line_idx, int offset) {
            m_offset = offset;
            m_source_line = line_idx;
            m_is_data = false;
            m_bytes = 0;
            m_ptr = nullptr;
        }

        ~Chunk();

        void print() const;

        /** Are we representing data? */
        [[nodiscard]] bool is_data() const { return m_is_data; }

        [[nodiscard]] int get_offset() const { return m_offset; }

        [[nodiscard]] int get_bytes() const { return m_bytes; }

        [[nodiscard]] int get_source_line() const { return m_source_line; }

        /** Interpret data as an instruction. */
        [[nodiscard]] instruction::Instruction *get_instruction() const { return (instruction::Instruction *) m_ptr; }

        /** Set data as instruction. */
        void set_instruction(instruction::Instruction *instruction);

        /** Interpret data as data. */
        [[nodiscard]] std::vector<unsigned char> *get_data() const { return (std::vector<unsigned char> *) m_ptr; }

        /** Set data as instruction. */
        void set_data(std::vector<unsigned char> *data);

        void write(std::ostream& out) const;

    };
}
