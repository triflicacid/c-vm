#pragma once

#include "signature.hpp"

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
