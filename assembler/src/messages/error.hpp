#pragma once

#include <string>
#include <vector>
#include "message.hpp"

namespace assembler {
    enum ErrorType {
        Syntax = 1,
        UnknownMnemonic,
        BadArguments,
        UnknownDirective,
        UnknownLabel,
        InvalidLabel,
        FileNotFound,
        Internal = 10,
        Opcode,
    };

    class Error : public Message {
    private:
        ErrorType m_type;

    public:
        Error(int line, int col, assembler::ErrorType err);

        ErrorType get_error() { return m_type; }
    };
}
