#include "Message.h"
#include "util.h"
#include <string>
#include <iostream>

namespace language {
    Message::Message(Level level, int row, int col, int idx, int len, std::string file, std::string msg, std::string src) {
        m_level = level;
        m_row = row;
        m_col = std::max(0, col);
        m_len = std::max(1, len);
        m_idx = std::max(0, idx);
        m_file = std::move(file);
        m_msg = std::move(msg);
        m_src = std::move(src);
    }

    void Message::print_notice() {
        std::cout << "[" CONSOLE_BLUE "NOTICE" CONSOLE_RESET "] " << m_file << ':' << m_row + 1 << ':' << m_col + 1 << ": " << m_msg
                  << '\n' << m_row + 1 << " | " << m_src.substr(0, m_idx) << CONSOLE_BLUE << m_src.substr(m_idx, m_len) << CONSOLE_RESET << m_src.substr(m_idx + m_len)
                  << '\n' << std::string(std::to_string(m_row).length(), ' ') << "   " << std::string(m_idx, ' ') << CONSOLE_BLUE "^" << std::string(m_len - 1, '~') << CONSOLE_RESET
                  << std::endl;
    }

    void Message::print_warning() {
        std::cout << "[" CONSOLE_YELLOW "WARNING" CONSOLE_RESET "] " << m_file << ':' << m_row + 1 << ':' << m_col + 1 << ": " << m_msg
                  << '\n' << m_row + 1 << " | " << m_src.substr(0, m_idx) << CONSOLE_YELLOW << m_src.substr(m_idx, m_len) << CONSOLE_RESET << m_src.substr(m_idx + m_len)
                  << '\n' << std::string(std::to_string(m_row).length(), ' ') << "   " << std::string(m_idx, ' ') << CONSOLE_YELLOW "^" << std::string(m_len - 1, '~') << CONSOLE_RESET
                  << std::endl;
    }

    void Message::print_error() {
        std::cout << "[" CONSOLE_RED "ERROR" CONSOLE_RESET "] " << m_file << ':' << m_row + 1 << ':' << m_col + 1 << ": " CONSOLE_YELLOW << m_msg << CONSOLE_RESET
                  << '\n' << m_row + 1 << " | ";

        if (m_idx >= m_src.length()) {
            std::cout << m_src << CONSOLE_RED_BG << std::string(m_len, ' ') << CONSOLE_RESET;
        } else {
            std::cout << m_src.substr(0, m_idx) << CONSOLE_RED << m_src.substr(m_idx, m_len) << CONSOLE_RESET
                      << m_src.substr(m_idx + m_len);
        }

        std::cout << '\n' << std::string(std::to_string(m_row).length(), ' ') << "   " << std::string(m_idx, ' ') << CONSOLE_RED "^" << std::string(m_len - 1, '~') << CONSOLE_RESET
                  << std::endl;
    }

    void Message::print() {
        switch (m_level) {
            case Level::notice:
                print_notice();
                return;
            case Level::warning:
                print_warning();
                return;
            case Level::error:
                print_error();
                return;
        }
    }
}
