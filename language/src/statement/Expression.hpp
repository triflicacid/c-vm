#pragma once

#include "Statement.hpp"
#include "lexer/Token.hpp"
#include "types/Type.hpp"

namespace language::statement {
    class Expression : public Statement {
    protected:
        const lexer::Token *m_token;

    public:
        explicit Expression(int pos, const lexer::Token *token) : Statement(pos), m_token(token) {};

        [[nodiscard]] const lexer::Token *token() const { return m_token; }

        [[nodiscard]] Type type() const override { return Type::EXPRESSION; }

        /** Get overall type of expression. */
        [[nodiscard]] virtual const types::Type *get_type_of() const { return nullptr; }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            stream << prefix << "<Expression>" << m_token->image() << "</Expression>";
        }
    };
}
