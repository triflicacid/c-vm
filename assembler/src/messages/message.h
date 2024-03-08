#pragma once

#include <string>
#include <vector>

namespace assembler {
    enum MessageLevel {
        Note,
        Warning,
        Error
    };

    class Message {
    private:
        int m_line;
        int m_col;
        MessageLevel m_type;

        /** Print varying type line e.g., 'ERROR!' */
        void print_type_suffix();

    public:
        std::string m_msg;

        Message(MessageLevel level, int line, int col);

        MessageLevel get_level() { return m_type; }

        void print();
    };
}
