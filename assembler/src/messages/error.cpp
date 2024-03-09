#include "error.hpp"

#include <string>
#include <utility>

namespace assembler {
    Error::Error(std::filesystem::path file, int line, int col, assembler::ErrorType err) : Message(MessageLevel::Error, std::move(file), line, col) {
        m_type = err;
    }
}
