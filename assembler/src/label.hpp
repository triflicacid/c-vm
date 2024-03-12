#pragma once

#include <string>

namespace assembler {
    struct Label {
        int line;
        int col;
        long long addr;
    };
}
