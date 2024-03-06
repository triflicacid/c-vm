#pragma once

#include <string>
#include <vector>
#include "Data.h"
#include "Message.h"
#include "tokens/NumericLiteral.h"
#include "tokens/BinaryOperator.h"
#include "types/NumericType.h"
#include "operators/Operator.h"

namespace language::parser {
    class Parser {
    private:
        Data *m_data = nullptr;

        void add_msg(Message::Level level, std::string msg, int idx, int len);

        /** Parse a numeric token type, return literal's end index, add to error. */
        int parse_numeric_type(const std::string &source, int idx, types::NumericType **type);

        /** Return if we should start scanning for a numeric literal */
        bool expect_numeric_lit(const std::string &source, int idx);

        /** Given a string, return the ending index and populate the pointer with the literal. */
        int parse_numeric_lit(const std::string &source, int idx_start, tokens::NumericLiteral **lit);

        /** Return end index of variable name */
        int parse_symbol_name(const std::string &source, int idx_start);

        /** Parse an operator, return end index */
        int parse_operator(const std::string &source, int idx, operators::Operator::Type &type);

        /** Parse an expression, populate argument and return end index */
        int parse_expression(const std::string &source, int idx_start, tokens::Token **token);

    public:
        /** Parse the input string. Remember to check <data> for any messages */
        void parse(Data *data, const std::string& source);
    };
}
