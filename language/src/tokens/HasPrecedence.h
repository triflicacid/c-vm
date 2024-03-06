#pragma once

#include "Token.h"

namespace language::tokens {
    class HasPrecedence : public Token {
    public:
        HasPrecedence(const std::string &source, int row, int idx, int len)
        : Token(source, row, idx, len) {};

        virtual int get_precedence() {
            return -1;
        }
    };
}