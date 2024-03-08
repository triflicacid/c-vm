#pragma once

#include <string>

namespace assembler::pre_processor {
    struct Constant {
        int line;
        int col;
        std::string value;
    };
}
