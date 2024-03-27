#include <iostream>
#include "disassembler.hpp"
#include "messages/error.hpp"
extern "C" {
#include "util.h"
}

namespace disassembler {
    /** Disassembler given data. */
    void disassemble(Data &data, message::List &msgs) {
        int pos = 0;

        // Skip past starting position
        pos += sizeof(WORD_T);

        // Extract address of .section DATA
        WORD_T data_section = *(WORD_T *)(data.buffer + pos);
        pos += sizeof(WORD_T);

        if (data.debug)
            std::cout << ".section DATA at +" << data_section << "\n";

        // TODO
        auto msg = new class message::Error(data.file_path, -1, pos, message::ErrorType::None);
        msg->set_message("Not Implemented.");
        msgs.add(msg);
    }
}
