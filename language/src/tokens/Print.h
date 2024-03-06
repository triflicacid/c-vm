#pragma once

#include <string>
#include "Token.h"
#include <iostream>
#include <utility>

namespace language::tokens {
    class Print : public Token {
    private:
        Token *m_body;

        // DEBUG
        void _debug_print(int indent) override {
            std::cout << "Print:";
            if (m_body) {
                std::cout << std::endl;
                m_body->debug_print(indent + 1);
            } else {
                std::cout << " [empty]" << std::endl;
            }
        }

    public:
        Print(const std::string &source, int row, int idx, int len)
        : Token(source, row, idx, len) {};

        /** Set body */
        void set_body(Token *body) {
            m_body = body;
        }

        Token::Type get_token_type() override {
            return Token::Type::print_expression;
        }
    };
}
