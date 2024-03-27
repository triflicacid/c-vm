#pragma once

#include "disassembler_data.hpp"
#include "messages/list.hpp"

namespace disassembler {
    /** Disassembler given data. */
    void disassemble(Data &data, message::List &msgs);
}
