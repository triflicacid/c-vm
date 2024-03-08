#pragma once

#include <string>
#include <vector>

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

    class Error {
    private:
        int m_line;
        int m_col;
        ErrorType m_type;
        std::vector<std::string> m_notes;


    public:
        std::string m_msg;

        Error(int line, int col, ErrorType type);

        void add_note(const std::string& note);

        ErrorType get_type() { return m_type; }

        void print() const;
    };
}
