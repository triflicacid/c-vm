#pragma once

#include "processor/src/opcodes.h"
#include "argument.hpp"
#include "signature.hpp"

#include <string>
#include <utility>
#include <vector>

namespace assembler::instruction {
    class Instruction {
    public:
        std::string mnemonic;
        OPCODE_T opcode;
        int bytes; // Length of instruction
        std::vector<Argument> args;

        Instruction(const Signature &signature, std::vector<Argument> arguments);

        void print();
    };
}
