#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "../pre-process/location-info.hpp"

namespace assembler::message {
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

        /** Print varying type line e.g., 'ERROR!' */
        void print_type_suffix();

    public:
        std::string m_msg;

        Message(Level level, std::filesystem::path filename, int line, int col);

        Message(Level level, const pre_processor::LocationInformation &loc);

        Level get_level() { return m_type; }

        void print();
    };
}
