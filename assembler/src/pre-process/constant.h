#pragma once

#include <string>

namespace assembler {
    struct Constant {
        int line;
        int col;
        std::string value;
    };
}
