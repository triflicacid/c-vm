#pragma once

#include "data.hpp"
#include "messages/list.hpp"

namespace assembler::parser {
    /** Parse lines into chunks. */
    void parse(Data &data, message::List &msgs);

    /** Parse an argument, populate <argument>. */
    void parse_arg(const Data &data, int line_idx, int &col, message::List &msgs, instruction::Argument &argument);

    /** Given a string, return argument type and value - register, literal, label (lit). User must check if end character is valid. */
    void parse_arg_lit(const Data &data, int line_idx, int &col, message::List &msgs, instruction::Argument &argument);

    /** Given a string, return register offset, or -1. */
    int parse_register(const std::string& string, int &index);
}
