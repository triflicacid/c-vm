#pragma once

#include "message.hpp"

namespace message {
    enum ErrorType {
        None,
        Syntax,
        UnknownMnemonic,
        BadArguments,
        UnknownDirective,
        UnknownLabel,
        InvalidLabel,
        FileNotFound,
        CircularInclude,
        UnknownSection,
        SectionSeenBefore,
    };

    class Error : public Message {
    private:
        ErrorType m_type;

    public:
        Error(std::filesystem::path file, int line, int col, ErrorType err);

        int get_code() override { return (int) m_type; }
    };
}
