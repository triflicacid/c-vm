#pragma once

#include "Token.h"
#include "HasPrecedence.h"
#include <map>

namespace language::tokens {
    class OpenBracketLiteral : public HasPrecedence {
    public:
        OpenBracketLiteral(const std::string &source, int row, int idx, int len)
        : HasPrecedence(source, row, idx, len) {};

        Token::Type get_token_type() override {
            return Token::Type::open_bracket_literal;
        }

        int get_precedence() override {
            return 4;
        }
    };

    class ClosedBracketLiteral : public Token {
    public:
        ClosedBracketLiteral(const std::string &source, int row, int idx, int len)
        : Token(source, row, idx, len) {};

        Token::Type get_token_type() override {
            return Token::Type::closed_bracket_literal;
        }
    };
}
