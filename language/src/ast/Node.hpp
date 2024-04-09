#pragma once

#include "util/Location.hpp"

namespace language::ast {
    class Node {
    public:
        enum class Type {
           Function,
        };

    protected:
        Location m_location; // Definition location
        Type m_type;

    public:
        Node(Location location, Type type) : m_location(location), m_type(type) {};
    };
}
