#pragma once

#include "HasPrecedence.h"
#include <map>
#include <iostream>
#include "operators/Operator.h"

namespace language::tokens {
    class BinaryOperator : public HasPrecedence {
    private:
        operators::Operator::Type m_type;
        Token *m_left = nullptr;
        Token *m_right = nullptr;

        // DEBUG
        void _debug_print(int indent) override;

    public:
        BinaryOperator(const std::string &source, int row, int idx, int len, operators::Operator::Type type)
        : HasPrecedence(source, row, idx, len), m_type(type) {};

        [[nodiscard]] operators::Operator::Type get_type() const { return m_type; }

        void set_left(tokens::Token *token) {
            m_left = token;
        }

        void set_right(tokens::Token *token) {
            m_right = token;
        }

        Token::Type get_token_type() override {
            return Token::Type::binary_operator;
        }

        int get_precedence() override {
            auto it = operators::Operator::precedence.find(m_type);

            if (it == operators::Operator::precedence.end()) {
                std::cerr << "get_precedence: cannot get precedence of operator type " << m_type << std::endl;
                std::exit(1);
            }

            return it->second;
        }
    };
}
