#pragma once

#include <sstream>
#include "Message.h"

namespace language::parser {
    class Data;
}

namespace language::tokens {
    class Token {
    public:
        enum Type {
            generic,
            binary_operator,
            numeric_literal,
            symbol_declaration,
            open_bracket_literal,
            closed_bracket_literal,
            symbol_reference,
            print_expression
        };

    private:
        // Location in source code
        const std::string &src_name;
        int src_row;
        int src_idx;
        int src_len;

        // DEBUG
        virtual void _debug_print(int indent);
    public:

        Token(const std::string &source, int row, int idx, int len)
        : src_name(source), src_row(row), src_idx(idx), src_len(len) {};

        /** Add a message to data about this token */
        void add_message(parser::Data *data, Message::Level level, std::string msg);

        // DEBUG
        void debug_print(int indent = 0);

        [[nodiscard]] virtual Type get_token_type() {
            return Type::generic;
        }
    };
}