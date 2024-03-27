#include <iostream>

#include "message.hpp"
extern "C" {
#include "util.h"
}

namespace message {
    Message::Message(Level level, std::filesystem::path filename, int line, int col) {
        m_type = level;
        m_line = line;
        m_col = col;
        m_file = std::move(filename);
    }

    Message::Message(Level level, const assembler::pre_processor::LocationInformation& loc) {
        m_type = level;
        m_line = loc.line;
        m_col = loc.col;
        m_file = loc.file;
    }

    void Message::_set_message(std::string msg) {
        m_msg = std::move(msg);
    }

    void Message::print_type_suffix() {
        switch (m_type) {
            case Level::Note:
                std::cout << CONSOLE_BLUE "NOTE" CONSOLE_RESET;
                break;
            case Level::Warning:
                std::cout << CONSOLE_YELLOW "WARN" CONSOLE_RESET;
                break;
            case Level::Error:
                std::cout << CONSOLE_RED "ERROR!" CONSOLE_RESET;
                break;
        }

        int code = get_code();
        if (code != -1) {
            std::cout << " [" << code << "]";
        }
    }

    void Message::set_message(const std::string &msg) {
        _set_message(msg);
    }

    void Message::set_message(const std::stringstream& stream) {
        _set_message(stream.str());
    }

    void Message::print() {
        print_type_suffix();

        std::cout << " File " << m_file.string();

        if (m_line > -1) {
            std::cout << ", line " << m_line;

            if (m_col > -1)
                std::cout << ", column " << m_col;
        } else if (m_col > -1) {
            std::cout << ", offset +" << m_col;
        }

        std::cout << ": " << m_msg << "\n";
    }
}
