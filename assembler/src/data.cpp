#include "data.hpp"

namespace assembler {
    void Data::replace_label(const std::string &label, int address) {
        for (const auto &chunk : chunks) {
            if (!chunk->is_data) {
                for (auto &arg : chunk->get_instruction()->args) {
                    if (arg.is_label() && *arg.get_label() == label) {
                        arg.transform_label(address);
                    }
                }
            }
        }
    }
}
