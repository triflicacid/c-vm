#include <iostream>
#include <string>
#include <utility>

#include "message.hpp"
#include "util.h"

namespace assembler::message {
    Message::Message(Level level, std::filesystem::path filename, int line, int col) {
        m_type = level;
        m_line = line;
        m_col = col;
        m_file = std::move(filename);
    }

    Message::Message(Level level, const pre_processor::LocationInformation& loc) {
        m_type = level;
        m_line = loc.line;
        m_col = loc.col;
        m_file = loc.file;
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
    }

    void Message::print() {
        print_type_suffix();

        std::cout << " File " << m_file.string();

        if (m_line > -1) {
            std::cout << ", line " << m_line;

            if (m_col > -1)
                std::cout << ", column " << m_col;
        }

        std::cout << ": " << m_msg << "\n";
    }
}
