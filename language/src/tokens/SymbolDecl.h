#pragma once

#include "Token.h"
#include "NumericLiteral.h"
#include <string>
#include <utility>

namespace language::tokens {
    class SymbolDeclaration : public Token {
    private:
        std::string m_symbol;
        types::Type *m_type;

        // DEBUG
        void _debug_print(int indent) override;

    public:
        SymbolDeclaration(std::string &source, int row, int idx, int len, std::string symbol, types::Type *type)
        : Token(source, row, idx, len), m_symbol(std::move(symbol)), m_type(type) {};

        Token::Type get_token_type() override {
            return Token::Type::symbol_declaration;
        }

        [[nodiscard]] types::Type *get_symbol_type() {
            return m_type;
        }
    };
}

