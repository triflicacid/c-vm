#include "assembler_data.hpp"
extern "C" {
#include "util.h"
}

namespace assembler {
    void Data::replace_label(const std::string &label, int address) {
        for (const auto &chunk : chunks) {
            if (!chunk->is_data()) {
                for (auto &arg : chunk->get_instruction()->args) {
                    if (arg.is_label() && *arg.get_label() == label) {
                        arg.transform_label(address);
                    }
                }
            }
        }
    }

    int Data::get_bytes() {
        if (chunks.empty())
            return 0;

        auto last = chunks.back();
        return last->get_offset() + last->get_bytes();
    }

    void Data::write_headers(std::ostream &stream) {
        if (chunks.empty())
            return;

        // Write start address
        auto start_label = labels.find(main_label);
        WORD_T start_addr = start_label == labels.end() ? 0 : start_label->second.addr;
        stream.write((char *) &start_addr, sizeof(start_addr));
    }

    void Data::write_chunks(std::ostream& stream) {
        for (auto chunk : chunks) {
            chunk->write(stream);
        }
    }
}
