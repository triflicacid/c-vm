#pragma once

#include <string>
#include <vector>

#include "line.h"
#include "err.h"

namespace assembler {
    namespace pre_processor {
        struct Data {
            std::string file_name;
            bool debug;
            std::vector<Line> lines;

            explicit Data(bool debug) {
                this->debug = debug;
            }
        };
    }

    /** Read source file into lines. Mutate `data`, or populate `err`. */
    void read_source_file(const std::string& filename, pre_processor::Data *data, Error **err);
}
