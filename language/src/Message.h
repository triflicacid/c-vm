#pragma once

#include <string>

namespace language {
    class Message {
    public:
        enum Level {
            notice,
            warning,
            error
        };

    private:
        Level m_level;
        int m_row; // Source row
        int m_col; // Source column
        int m_len; // Length of the error
        int m_idx; // Index to point to
        std::string m_file;
        std::string m_msg;
        std::string m_src;

    public:
        Message(Level level, int row, int col, int idx, int len, std::string file, std::string msg, std::string src);

        void print_notice();
        void print_warning();
        void print_error();
        void print();

        [[nodiscard]] Message::Level get_level() const { return m_level; }
        [[nodiscard]] std::string get_message() const { return m_msg; }
    };
}
