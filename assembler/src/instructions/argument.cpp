#include "argument.hpp"
extern "C" {
#include "util.h"
}

#include <iostream>
#include <string>
#include <iomanip>

namespace assembler::instruction {
    Argument::Argument(ArgumentType type, unsigned long long data) {
        this->m_type = type;
        this->m_data = data;
    }

    bool Argument::is_label() {
        return m_type == ArgumentType::LabelAddress || m_type == ArgumentType::LabelLiteral;
    }

    void Argument::print(std::ostream &out) {
        switch (m_type) {
            case ArgumentType::Literal:
                out << "literal " << m_data << " {" << std::hex;

                for (int off = 0; off < sizeof(m_data); ++off) {
                    out << std::setw(2) << std::setfill('0') << ((m_data >> (off * 8)) & 0xFF);

                    if (off < sizeof(m_data) - 1)
                        out << " ";
                }

                out << std::dec << "}";
                break;
            case ArgumentType::Address:
                out << "address " << m_data;
                break;
            case ArgumentType::Register:
                out << "register " << m_data;
                break;
            case ArgumentType::RegisterPointer:
                out << "register pointer " << m_data;
                break;
            case ArgumentType::LabelLiteral:
                out << "label (lit.) \"" << *(std::string *) m_data << "\"";
                break;
            case ArgumentType::LabelAddress:
                out << "label (addr.) \"" << *(std::string *) m_data << "\"";
                break;
        }
    }

    void Argument::set_label(const std::string &label) {
        auto ptr = new std::string(label);
        m_data = (unsigned long long) ptr;
    }

    void Argument::transform_label(unsigned long long value) {
        if (m_type == ArgumentType::LabelAddress) {
            delete (std::string *) m_data;
            m_type = ArgumentType::Address;
            m_data = value;
        } else if (m_type == ArgumentType::LabelLiteral) {
            delete (std::string *) m_data;
            m_type = ArgumentType::Literal;
            m_data = value;
        }
    }

    void Argument::update(ArgumentType type, unsigned long long int data) {
        if (is_label()) {
            delete (std::string *) data;
        }

        m_type = type;
        m_data = data;
    }

    void Argument::transform_address_equivalent() {
        switch (m_type) {
            case ArgumentType::Literal:
                m_type = ArgumentType::Address;
                break;
            case ArgumentType::Register:
                m_type = ArgumentType::RegisterPointer;
                break;
            case ArgumentType::LabelLiteral:
                m_type = ArgumentType::LabelAddress;
                break;
            default:
                break;
        }
    }
}
