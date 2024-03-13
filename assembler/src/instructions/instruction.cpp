#include <iostream>
#include <utility>
#include "instruction.hpp"

namespace assembler::instruction {
    void Instruction::print() {
        std::cout << "Mnemonic \"" << signature->get_mnemonic() << "\"; Opcode = " << std::hex << signature->get_opcode() << std::dec << "; "
            << get_bytes() << " bytes; " << args.size() << " argument(s)\n";

        for (Argument arg : args) {
            std::cout << "\t- ";
            arg.print();
            std::cout << '\n';
        }
    }

    Instruction::Instruction(Signature &signature, std::vector<Argument> arguments) {
        this->signature = &signature;
        args = std::move(arguments);
    }

    int Instruction::get_bytes() const {
        return signature ? signature->get_bytes() : 0;
    }

    void Instruction::write(std::ostream &stream) {
        auto opcode = signature->get_opcode();
        stream.write((char *) &opcode, sizeof(opcode));

        for (int i = 0; i < signature->param_count(); i++) {
            auto param = signature->get_param(i);
            auto arg_data = args[i].get_data();

            stream.write((char *) &arg_data, param->size);
        }
    }
}
