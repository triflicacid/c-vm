#include <iostream>

#include "err.h"
#include "util.h"

namespace assembler {
    Error::Error(int line, int col, assembler::ErrorType type) {
        m_line = line;
        m_col = col;
        m_type = type;
    }

    void Error::add_note(const std::string& note) {
        m_notes.push_back(note);
    }

    void Error::print() const {
        std::cout << CONSOLE_RED "ERROR!" CONSOLE_RESET;

        if (m_line > -1) {
            std::cout << " Line " << m_line;

            if (m_col > -1)
                std::cout << ", column " << m_col;

            std::cout << ": ";
        } else {
            std::cout << " ";
        }

        std::cout << m_msg << "\n";

        for (const auto& note : m_notes) {
            std::cout << CONSOLE_BLUE "NOTE" << CONSOLE_RESET << " " << note << "\n";
        }
    }
}
