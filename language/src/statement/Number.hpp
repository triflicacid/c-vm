#pragma once

#include "Expression.hpp"
#include "types/NumericType.hpp"

namespace language::statement {
    class Number : public Expression {
    private:
        const types::NumericType *m_type;

    public:
        Number(int pos, const lexer::Token *token, lexer::Token::Type numeric_type)
        : Expression(pos, token) {
            m_type = &types::num_types.find(numeric_type)->second;
        };

        [[nodiscard]] Type type() const override { return Type::NUMBER; }

        [[nodiscard]] size_t size() const { return m_type->size(); }

        [[nodiscard]] const types::Type *get_type_of() const override { return m_type; }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            stream << prefix << "<Number>" << std::endl;
            m_type->debug_print(stream, prefix + "  ");
            stream << std::endl << prefix << "  <Value>" << m_token->image() << "</Value>" << std::endl;
            stream << prefix << "</Number>";
        }
    };
}
