#pragma once

#include <string>
#include <utility>
#include <iostream>
#include "Token.h"
#include "types/NumericType.h"

namespace language::tokens {
    class NumericLiteral : public Token {
    private:
        std::string m_content;
        types::NumericType *m_type;

        // DEBUG
        void _debug_print(int indent) override {
            std::cout << "Numeric literal '" << m_content << "' of type @:" << m_type << " (sizeof=" << m_type->size_of() << ")" << std::endl;
        }

    public:
        NumericLiteral(std::string &source, int row, int idx, int len, std::string data, types::NumericType *type)
        : Token(source, row, idx, len), m_content(std::move(data)), m_type(type) {};

        virtual Token::Type get_token_type() override {
            return Token::Type::numeric_literal;
        }

        [[nodiscard]] std::string get_data() {
            return m_content;
        }

        [[nodiscard]] types::NumericType *get_type() {
            return m_type;
        }
    };
}
