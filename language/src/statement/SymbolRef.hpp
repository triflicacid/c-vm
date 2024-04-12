#pragma once

#include "Expression.hpp"
#include "parser/SymbolDeclaration.hpp"

namespace language::statement {
    class SymbolRef : public Expression {
    private:
        const parser::SymbolDeclaration *m_decl;

    public:
        SymbolRef(int pos, const lexer::Token *token, const parser::SymbolDeclaration *decl)
        : Expression(pos, token), m_decl(decl) {};

        [[nodiscard]] Type type() const override { return Type::SYMBOL_REF; }

        [[nodiscard]] std::string name() const { return m_token->image(); }

        [[nodiscard]] const parser::SymbolDeclaration *decl() const { return m_decl; }

        [[nodiscard]] const types::Type *get_type_of() const override { return m_decl->type(); }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            m_decl->debug_print(stream, prefix);
        }
    };
}
