#pragma once

#include <map>

#include "constant.hpp"
#include "line.hpp"
#include "macro.hpp"
#include "location-info.hpp"

namespace assembler::pre_processor {
    struct Data {
        std::filesystem::path executable; // Path to executable
        std::filesystem::path file_path;  // Name of source file
        bool debug;               // Print debug comments?
        std::vector<Line> lines;  // List of source file lines
        std::map<std::string, Constant> constants; // Map of constant values (%define)
        std::map<std::string, Macro> macros; // Map of macros
        std::map<std::filesystem::path, LocationInformation> included_files; // Maps included files to where they were included

        explicit Data(bool debug) {
            this->debug = debug;
        }

        Data(const Data& data) {
            debug = data.debug;
            file_path = data.file_path;
            executable = data.executable;
        }

        /** Set executable path. */
        void set_executable(const std::string& path);

        /** Writes `lines` to buffer. */
        std::string write_lines();

        /** Merge given data with this. Insert lines starting at `index`. */
        void merge(struct Data &other, int line_index = -1);
    };
}
