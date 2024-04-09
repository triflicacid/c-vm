#pragma once

#include "Type.hpp"
#include <ostream>

namespace language::types {
    class NumericType : public Type {
    private:
        lexer::NumInfo *m_num;

    public:
        NumericType(lexer::Token *token, int token_pos)
        : Type(token, token_pos) {
            m_num = &lexer::num_types.find(token->type())->second;
        };

        [[nodiscard]] lexer::NumInfo *info() const { return m_num; }

        [[nodiscard]] size_t size() const override { return m_num->bytes; }

        [[nodiscard]] Category category() const override { return Category::Numeric; }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            stream << prefix << "<NumericType bytes=\"" << m_num->bytes << "\" signed=\"" << m_num->is_signed << "\" float=\""
                << m_num->is_float << "\">" << m_token->image() << "</NumericType>";
        }
    };
}
