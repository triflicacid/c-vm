#pragma once

#include <string>
#include <vector>
#include <map>

#include "line.h"
#include "err.h"

namespace assembler {
    namespace pre_processor {
        struct Data {
            std::string file_name;    // Name of source file
            bool debug;               // Print debug comments?
            std::vector<Line> lines;  // List of source file lines
            std::map<std::string, std::string> constants; // Map of constant values (%define)

            explicit Data(bool debug) {
                this->debug = debug;
            }

            /** Writes `lines` to buffer. */
            std::string write_lines();
        };
    }

    /** Read source file into lines. Mutate `data`, or populate `err`. */
    void read_source_file(const std::string& filename, pre_processor::Data *data, Error **err);

    /** Run pre-processing on the given data, mutating it, or populate `err`. */
    void pre_process(pre_processor::Data *data, Error **err);
}
