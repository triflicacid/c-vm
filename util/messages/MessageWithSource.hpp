#pragma once

#include "message.hpp"

#include <string>

/** A more in-depth message class able to handle source information. No separate error class. */
namespace message {
    class MessageWithSource : public Message {
    private:
        int m_len; // Length of the error
        int m_idx; // Index to point to
        std::string m_src;

    public:
        MessageWithSource(Level level, std::filesystem::path filename, int line, int col, int idx, int len, const std::string& src);

        void print_notice();
        void print_warning();
        void print_error();
        void print() override;
    };
}
