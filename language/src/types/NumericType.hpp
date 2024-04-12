#pragma once

#include "Type.hpp"
#include <ostream>

namespace language::types {
    class NumericType : public Type {
    private:
        lexer::Token::Type m_type;
        int m_bits;
        bool m_signed;
        bool m_is_float;

    public:
        explicit NumericType(lexer::Token::Type token_type, int bits, bool is_signed, bool is_float)
        : Type(), m_type(token_type), m_bits(bits), m_signed(is_signed), m_is_float(is_float) {};

        [[nodiscard]] size_t size() const override { return m_bits / 8; }

        [[nodiscard]] Category category() const override { return Category::Numeric; }

        [[nodiscard]] bool is_single_instance() const override { return true; }

        [[nodiscard]] std::string repr() const override { return lexer::tokens[m_type]; }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            stream << prefix << "<NumericType bits=\"" << m_bits << "\" signed=\"" << m_signed << "\" float=\""
                << m_is_float << "\">" << repr() << "</NumericType>";
        }

        /** Return: can fst be implicitly casted to snd? I.e., both integers and snd>=fst. */
        static bool can_implicitly_cast_to(const NumericType *fst, const NumericType *snd) {
            return !fst->m_is_float && !snd->m_is_float && snd->m_bits >= fst->m_bits;
        }

        /** Get smallest NumericType which will fit said integer. */
        static lexer::Token::Type get_suitable_type(unsigned long long);
    };

    /** Checks if both types are numeric and can cast from fst to snd. */
    bool can_implicitly_cast_to(const Type *fst, const Type *snd);

    extern std::map<lexer::Token::Type, NumericType> num_types;
}
