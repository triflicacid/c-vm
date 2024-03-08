#pragma once

namespace assembler {
    enum ArgumentType {
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
        Argument(ArgumentType type, unsigned long long data);

        ArgumentType get_type() { return m_type; }

        [[nodiscard]] unsigned long long get_data() const { return m_data; }

        /** Can this argument be used as a literal? */
        bool is_literal();

        /** Can this argument be used as an address? */
        bool is_address();

        /** Is this argument a label? */
        bool is_label();

        /** Print the label. */
        void print();
    };
}
