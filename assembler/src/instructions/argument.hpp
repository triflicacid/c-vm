#pragma once

#include <string>
#include <iostream>

namespace assembler::instruction {
    enum class ArgumentType {
        Literal,
        Address,
        Register,
        RegisterPointer,
        LabelLiteral,
        LabelAddress
    };

    class Argument {
    private:
        ArgumentType m_type;
        unsigned long long m_data;

    public:
        Argument() {
            m_type = ArgumentType::Literal;
            m_data = 0;
        }

        Argument(ArgumentType type, unsigned long long data);

        [[nodiscard]] ArgumentType get_type() const { return m_type; }

        [[nodiscard]] unsigned long long get_data() const { return m_data; }

        /** Interpret `data` as a label. */
        [[nodiscard]] std::string *get_label() const { return (std::string *) m_data; };

        [[nodiscard]] int get_bytes() const { return sizeof(m_data); }

        void update(ArgumentType type, unsigned long long data);

        /** Transform type: update ton address-equivalent. E.g., '42' -> '[42]' */
        void transform_address_equivalent();

        /** Can this argument be used as a literal? */
        bool is_literal();

        /** Can this argument be used as an address? */
        bool is_address();

        /** Is this argument a label? */
        bool is_label();

        /** Set value to a label. */
        void set_label(const std::string &label);

        /** Transform label to constant with the given value. */
        void transform_label(unsigned long long value);

        /** Print the label. */
        void print(std::ostream& out = std::cout);
    };
}
