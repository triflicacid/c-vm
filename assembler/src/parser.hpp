#pragma once

#include "data.hpp"
#include "messages/list.hpp"

/** (v_int, v_dbl, is_dbl) */
using AddBytesFunction = std::function<void(unsigned long long, double, bool)>;

namespace assembler::parser {
    /** Parse lines into chunks. */
    void parse(Data &data, message::List &msgs);

    /** Parse constant "<type>: <sequence>". Create byte vector on heap. Return if success. */
    bool parse_data(const Data &data, int line_idx, int &col, message::List &msgs, std::vector<unsigned char> **bytes);

    /** Parse an argument, populate <argument>. */
    void parse_arg(const Data &data, int line_idx, int &col, message::List &msgs, instruction::Argument &argument);

    /** Given a string, return argument type and value - register, literal, label (lit). User must check if end character is valid. */
    void parse_arg_lit(const Data &data, int line_idx, int &col, message::List &msgs, instruction::Argument &argument);

    /** Given a string, return register offset, or -1. */
    int parse_register(const std::string& string, int &index);

    /** Parse numeric literal: int or float. Return if we did find a number. */
    bool parse_number(const std::string& string, bool& is_decimal, unsigned long long& v_int, double &v_dbl);

    /** Parse character. String assumed to have started with an apostrophe, with <index> pointing after this. */
    void parse_character_literal(const Data &data, int line_idx, int &col, message::List &msgs, unsigned long long& value);

    /** Given a string, add bytes to data that it represents. Parse only a single data item e.g. '42'. */
    void parse_byte_item(const Data &data, int line_idx, int &col, message::List &msgs, AddBytesFunction add_bytes);

    /** Given a string, add bytes to data that it represents. Parse an entire data string e.g., '42 0 '\0'' */
    void parse_byte_sequence(const Data &data, int line_idx, int &col, message::List &msgs, AddBytesFunction add_bytes);
}
