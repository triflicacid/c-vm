#pragma once

#include <string>
#include <utility>
#include <vector>

namespace assembler::pre_processor {
    struct Macro {
        int line;
        int col;
        std::vector<std::string> params;
        std::vector<std::string> lines; // Lines in macro's body

        Macro(int line, int col, std::vector<std::string> params) {
            this->line = line;
            this->col = col;
            this->params = std::move(params);
        }
    };
}
