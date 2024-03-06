#pragma once

#include <string>
#include "Token.h"
#include <iostream>
#include <utility>

namespace language::tokens {
    class SymbolReference : public Token {
    private:
        std::string m_symbol;
        int m_offset = 0;
        size_t m_size = 0;

        // DEBUG
        void _debug_print(int indent) override {
            std::cout << "Reference to symbol " << m_symbol << " (offset=" << m_offset << ", sizeof=" << m_size << ")" << std::endl;
        }

    public:
        SymbolReference(const std::string &source, int row, int idx, int len, std::string symbol)
        : Token(source, row, idx, len), m_symbol(std::move(symbol)) {};

        /** Set offset and size fields */
        void set_data(int offset, size_t size) {
            m_offset = offset;
            m_size = size;
        }

        Token::Type get_token_type() override {
            return Token::Type::symbol_reference;
        }
    };
}
