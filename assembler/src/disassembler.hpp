#pragma once

#include "disassembler_data.hpp"
#include "messages/list.hpp"

namespace disassembler {
    /** Disassembler given data. */
    void disassemble(Data &data, message::List &msgs);

    /** Write byte vector to stream. */
    void write_data_to_stream(std::stringstream &stream, const std::vector<unsigned char> &bytes, bool format);

    /** Write vector of printable characters. */
    void write_chars_to_stream(std::stringstream &stream, const std::vector<unsigned char> &chars);
}
