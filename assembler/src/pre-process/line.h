#pragma once

#include <string>

namespace assembler {
    struct Line {
        int n;
        std::string data;

        void print() const;
    };
}
