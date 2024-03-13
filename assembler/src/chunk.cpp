#include "chunk.hpp"
extern "C" {
#include "util.h"
}

#include <iostream>
#include <vector>
#include <iomanip>

namespace assembler {
    void Chunk::free_ptr() const {
        if (m_ptr != nullptr) {
            if (m_is_data) {
                delete get_data();
            } else {
                delete get_instruction();
            }
        }
    }

    Chunk::~Chunk() {
        free_ptr();
    }

    void Chunk::print() const {
        std::cout << "Chunk at +" << m_offset << " of " << m_bytes << " bytes";

        if (m_is_data) {
            std::cout << " - data:\n\t" << std::uppercase << std::hex;

            auto bytes = *get_data();

            for (int i = 0; i < bytes.size(); i++) {
                std::cout << std::setw(2) << std::setfill('0') << (int) bytes[i];

                if (i + 1 < bytes.size())
                    std::cout << " ";
            }

            std::cout << std::dec << "\n";
        } else {
            std::cout << " - instruction:\n\t";
            get_instruction()->print();
        }
    }

    int Chunk::get_in_range(const std::vector<Chunk>& chunks, int lower, int upper) {
        int idx = 0;
        for (const Chunk& chunk : chunks) {
            if (chunk.m_offset <= upper && chunk.m_offset + chunk.m_bytes > lower) {
                return idx;
            }

            idx++;
        }

        return -1;
    }

    void Chunk::set_instruction(instruction::Instruction *instruction) {
        free_ptr();

        m_is_data = false;
        m_bytes = instruction->get_bytes();
        m_ptr = instruction;
    }

    void Chunk::set_data(std::vector<unsigned char> *data) {
        free_ptr();

        m_is_data = true;
        m_bytes = (int) data->size();
        m_ptr = data;
    }

    void Chunk::write(std::ostream &out) const {
        if (m_is_data) {
            for (auto& byte : *get_data()) {
                out.write((char *) &byte, sizeof(byte));
            }
        } else {
            get_instruction()->write(out);
        }
    }
}
