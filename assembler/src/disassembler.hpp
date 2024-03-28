#pragma once

#include "disassembler_data.hpp"
#include "messages/list.hpp"
#include "instructions/signature.hpp"

namespace disassembler {
    /** Disassembler given data. */
    void disassemble(Data &data, message::List &msgs);

    /** Write data segment starting at an offset to the stream. Offset incremented by segment size. */
    void write_data_segment(Data &data, int &offset);

    /** Write byte vector to stream. */
    void write_data_to_stream(std::stringstream &stream, const std::vector<unsigned char> &bytes, bool format, bool commas);

    /** Write vector of printable characters. */
    void write_chars_to_stream(std::stringstream &stream, const std::vector<unsigned char> &chars);

    /** Write signature to stream. ptr will be advanced by signature.get_bytes(). */
    void write_signature_to_stream(Data &data, assembler::instruction::Signature &signature, int &ptr);

    /** Given a register offset, return string or "". */
    std::string register_to_string(int reg);

    /** Extract number of given size in bytes (<= 8). */
    unsigned long long extract_number(const char *buffer, int size, int ptr);
}
