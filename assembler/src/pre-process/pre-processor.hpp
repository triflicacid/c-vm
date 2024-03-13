#pragma once

#include "line.hpp"
#include "../messages/list.hpp"
#include "data.hpp"

namespace assembler {
    /** Read source file into lines. Mutate `data`, or add error. */
    void read_source_file(const std::string& filename, pre_processor::Data &data, message::List &msgs);

    /** Run pre-processing on the given data, mutating it, or add error. */
    void pre_process(pre_processor::Data &data, message::List &msgs);
}
