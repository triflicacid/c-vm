#include <iostream>
#include "instruction.hpp"


namespace assembler {
    void Instruction::print() {
        std::cout << "Mnemonic \"" << mnemonic << "\"; Opcode = " << std::hex << opcode << std::dec << "; "
            << bytes << " bytes; " << args.size() << " argument(s)\n";

        for (Argument arg : args) {
            std::cout << "\t- ";
            arg.print();
            std::cout << '\n';
        }
    }
}
