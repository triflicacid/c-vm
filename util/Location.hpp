#pragma once

#include <string>

class Location {
private:
    int m_line;
    int m_col;

public:
    /**
     * Create a new location vector.
     * @param line Line.
     * @param col Column. Set to `-1` if information it not available.
     */
    Location(int line, int col = -1) : m_line(line), m_col(col) {};

    int line() { return m_line; }

    int column() { return m_col; }

    std::string string();
};
