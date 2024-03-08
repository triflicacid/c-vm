#include "args.hpp"
extern "C" {
#include "util.h"
}

#include <iostream>
#include <string>

namespace assembler {
    Argument::Argument(ArgumentType type, unsigned long long data) {
        this->m_type = type;
        this->m_data = data;
    }

    bool Argument::is_address() {
        return m_type == ArgumentType::Address || m_type == ArgumentType::LabelAddress;
    }

    bool Argument::is_literal() {
        return m_type == ArgumentType::Literal || m_type == ArgumentType::LabelLiteral;
    }

    bool Argument::is_label() {
        return m_type == ArgumentType::LabelAddress || m_type == ArgumentType::LabelLiteral;
    }

    void Argument::print() {
        switch (m_type) {
            case ArgumentType::Literal:
                std::cout << "literal " << m_data << " {";
                print_bytes(&m_data, 8);
                std::cout << "\n}";
                break;
            case ArgumentType::Address:
                std::cout << "address " << m_data;
                break;
            case ArgumentType::Register:
                std::cout << "register " << m_data;
                break;
            case ArgumentType::RegisterPointer:
                std::cout << "register pointer " << m_data;
                break;
            case ArgumentType::LabelLiteral:
                std::cout << "label (lit.) \"" << *(std::string *) m_data << "\"";
                break;
            case ArgumentType::LabelAddress:
                std::cout << "label (addr.) \"" << *(std::string *) m_data << "\"";
                break;
        }
    }
}
