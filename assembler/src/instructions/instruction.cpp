#include <iostream>
#include "instruction.hpp"

namespace assembler::instruction {
    void Instruction::print() {
        std::cout << "Mnemonic \"" << mnemonic << "\"; Opcode = " << std::hex << opcode << std::dec << "; "
            << bytes << " bytes; " << args.size() << " argument(s)\n";

        for (Argument arg : args) {
            std::cout << "\t- ";
            arg.print();
            std::cout << '\n';
        }
    }

    Instruction::Instruction(const Signature &signature, std::vector<Argument> arguments) {
        mnemonic = signature.get_mnemonic();
        opcode = signature.get_opcode();
        bytes = (int) sizeof(opcode) + (int) arguments.size() * arguments[0].get_bytes();
        args = arguments;
    }
}
