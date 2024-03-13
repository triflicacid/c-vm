#include "error.hpp"

namespace assembler::message {
    Error::Error(std::filesystem::path file, int line, int col, ErrorType err) : Message(Level::Error, std::move(file), line, col) {
        m_type = err;
    }
}
