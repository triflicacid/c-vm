#pragma once

#include <string>
#include <vector>

#include "../pre-process/location-info.hpp"

namespace message {
    enum Level {
        Note,
        Warning,
        Error
    };

    class Message {
    private:
        int m_line;
        int m_col;
        std::filesystem::path m_file;
        Level m_type;
        std::string m_msg;

        /** Print varying type line e.g., 'ERROR!' */
        void print_type_suffix();

    protected:
        virtual void _set_message(std::string msg);

    public:
        Message(Level level, std::filesystem::path filename, int line, int col);

        Message(Level level, const assembler::pre_processor::LocationInformation &loc);

        std::string *get_message() { return &m_msg; }

        /** Get message code, or -1. */
        virtual int get_code() { return -1; }

        void set_message(const std::string& msg);

        void set_message(const std::stringstream& stream);

        Level get_level() { return m_type; }

        void print();
    };
}
