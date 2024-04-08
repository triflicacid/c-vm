#include "Location.hpp"

#include <sstream>

std::string Location::string() {
    std::stringstream stream;
    stream << "line " << m_line;

    if (m_col > -1) {
        stream << ", column " << m_col;
    }

    return stream.str();
}
