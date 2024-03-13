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
        Signature *signature;
        std::vector<Argument> args;

        Instruction(Signature &signature, std::vector<Argument> arguments);

        [[nodiscard]] int get_bytes() const;

        void write(std::ostream& stream);

        void print();
    };
}
