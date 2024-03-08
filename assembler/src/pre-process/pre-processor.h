#pragma once

#include <string>
#include <vector>
#include <map>

#include "line.h"
#include "assembler/src/messages/message-list.h"
#include "constant.h"

namespace assembler {
    namespace pre_processor {
        struct Data {
            std::string file_name;    // Name of source file
            bool debug;               // Print debug comments?
            std::vector<Line> lines;  // List of source file lines
            std::map<std::string, Constant> constants; // Map of constant values (%define)

            explicit Data(bool debug) {
                this->debug = debug;
            }

            /** Writes `lines` to buffer. */
            std::string write_lines();
        };
    }

    /** Read source file into lines. Mutate `data`, or add error. */
    void read_source_file(const std::string& filename, pre_processor::Data &data, MessageList &msgs);

    /** Run pre-processing on the given data, mutating it, or add error. */
    void pre_process(pre_processor::Data &data, MessageList &msgs);
}
