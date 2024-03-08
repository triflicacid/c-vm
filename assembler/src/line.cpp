#include "line.h"

#include <iostream>

namespace assembler {
    void Line::print() {
        std::cout << n << " | " << data << "\n";
    }
};
