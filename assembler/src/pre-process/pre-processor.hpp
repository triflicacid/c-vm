#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>

#include "line.hpp"
#include "../messages/message-list.hpp"
#include "constant.pph"
#include "macro.hpp"
#include "location-info.hpp"


namespace assembler {
    namespace pre_processor {
        struct Data {
            std::filesystem::path file_path;  // Name of source file
            bool debug;               // Print debug comments?
            std::vector<Line> lines;  // List of source file lines
            std::map<std::string, Constant> constants; // Map of constant values (%define)
            std::map<std::string, Macro> macros; // Map of macros
            std::map<std::filesystem::path, LocationInformation> included_files; // Maps included files to where they were included

            explicit Data(bool debug) {
                this->debug = debug;
            }

            /** Writes `lines` to buffer. */
            std::string write_lines();

            /** Merge given data with this. Insert lines starting at `index`. */
            void merge(struct Data &other, int line_index = -1);
        };
    }

    /** Read source file into lines. Mutate `data`, or add error. */
    void read_source_file(const std::string& filename, pre_processor::Data &data, MessageList &msgs);

    /** Run pre-processing on the given data, mutating it, or add error. */
    void pre_process(pre_processor::Data &data, MessageList &msgs);
}
