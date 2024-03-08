#include "line.h"

#include <iostream>

namespace assembler {
    void Line::print() const {
        std::cout << n << " | " << data << "\n";
    }
}
