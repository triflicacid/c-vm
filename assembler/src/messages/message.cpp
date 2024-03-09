#include <iostream>
#include <string>
#include <utility>

#include "message.hpp"
#include "util.h"

namespace assembler {
    Message::Message(MessageLevel level, std::filesystem::path filename, int line, int col) {
        m_type = level;
        m_line = line;
        m_col = col;
        m_file = std::move(filename);
    }

    void Message::print_type_suffix() {
        switch (m_type) {
            case MessageLevel::Note:
                std::cout << CONSOLE_BLUE "NOTE" CONSOLE_RESET;
                break;
            case MessageLevel::Warning:
                std::cout << CONSOLE_YELLOW "WARN" CONSOLE_RESET;
                break;
            case MessageLevel::Error:
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
