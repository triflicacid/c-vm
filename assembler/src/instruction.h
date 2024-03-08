#pragma once

#include "processor/src/opcodes.h"
#include "args.h"

#include <string>
#include <vector>

namespace assembler {
    class Instruction {
    public:
        std::string mnemonic;
        OPCODE_T opcode;
        int bytes; // Length of instruction
        std::vector<Argument> args;

        void print();
    };
}
