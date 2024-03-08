#include "error.h"

namespace assembler {
    Error::Error(int line, int col, assembler::ErrorType err) : Message(MessageLevel::Error, line, col) {
        m_type = err;
    }
}
