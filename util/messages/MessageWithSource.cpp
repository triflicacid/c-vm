#include "MessageWithSource.hpp"
#include "util.h"
#include <string>
#include <iostream>
#include <utility>

namespace message {
    MessageWithSource::MessageWithSource(Level level, std::filesystem::path filename, int line, int col, int idx, int len, const std::string& src)
    : Message(level, std::move(filename), line, col) {
        m_idx = std::max(0, idx);
        m_len = std::max(1, std::min(len, (int) src.length() - m_idx));
        m_src = src;
    }

    void MessageWithSource::print_notice() {
        std::cout << "[" CONSOLE_BLUE "NOTICE" CONSOLE_RESET "] " << m_file.string() << ':' << m_line + 1 << ':' << m_col + 1 << ": " << m_msg
                  << '\n' << m_line + 1 << " | " << m_src.substr(0, m_idx) << CONSOLE_BLUE << m_src.substr(m_idx, m_len) << CONSOLE_RESET << m_src.substr(m_idx + m_len)
                  << '\n' << std::string(std::to_string(m_line + 1).length(), ' ') << "   " << std::string(m_idx, ' ') << CONSOLE_BLUE "^" << std::string(m_len - 1, '~') << CONSOLE_RESET
                  << std::endl;
    }

    void MessageWithSource::print_warning() {
        std::cout << "[" CONSOLE_YELLOW "WARNING" CONSOLE_RESET "] " << m_file.string() << ':' << m_line + 1 << ':' << m_col + 1 << ": " << m_msg
                  << '\n' << m_line + 1 << " | " << m_src.substr(0, m_idx) << CONSOLE_YELLOW << m_src.substr(m_idx, m_len) << CONSOLE_RESET << m_src.substr(m_idx + m_len)
                  << '\n' << std::string(std::to_string(m_line + 1).length(), ' ') << "   " << std::string(m_idx, ' ') << CONSOLE_YELLOW "^" << std::string(m_len - 1, '~') << CONSOLE_RESET
                  << std::endl;
    }

    void MessageWithSource::print_error() {
        std::cout << "[" CONSOLE_RED "ERROR" CONSOLE_RESET "] " << m_file.string() << ':' << m_line + 1 << ':' << m_col + 1 << ": " CONSOLE_YELLOW << m_msg << CONSOLE_RESET
                  << '\n' << m_line + 1 << " | ";

        if (m_idx >= m_src.length()) {
            std::cout << m_src << CONSOLE_RED_BG << std::string(m_len, ' ') << CONSOLE_RESET;
        } else {
            std::cout << m_src.substr(0, m_idx) << CONSOLE_RED << m_src.substr(m_idx, m_len) << CONSOLE_RESET
                      << m_src.substr(m_idx + m_len);
        }


        std::cout << '\n' << std::string(std::to_string(m_line + 1).length(), ' ') << "   " << std::string(m_idx, ' ') << CONSOLE_RED "^" << std::string(m_len - 1, '~') << CONSOLE_RESET
                  << std::endl;
    }

    void MessageWithSource::print() {
        switch (m_level) {
            case Level::Note:
                print_notice();
                return;
            case Level::Warning:
                print_warning();
                return;
            case Level::Error:
                print_error();
                return;
        }
    }
}
