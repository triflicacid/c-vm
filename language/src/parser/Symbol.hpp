#pragma once

#include <string>
#include <ostream>

#include "util/Location.hpp"

namespace language::parser {
    /** Represents a symbol - tells us it exists, and what it is. */
    class Symbol {
    public:
        enum class Type {
            Variable, // Basic variable symbol
            Function,
        };

    private:
        Type m_type;
        int m_pos; // Declaration position (token offset)

    protected:
        std::string m_name;

    public:
        Symbol(Type type, int pos, const std::string& name) : m_type(type), m_pos(pos), m_name(name) {};

        [[nodiscard]] Type type() const { return m_type; }

        [[nodiscard]] int position() const { return m_pos; }

        [[nodiscard]] virtual size_t size() const { return 0; }

        [[nodiscard]] std::string name() const { return m_name; }

        /** Query: can this item be assigned to? */
        [[nodiscard]] virtual bool can_be_assigned() const { return false; }

        virtual void debug_print(std::ostream& stream, size_t offset, const std::string& prefix) const {
            stream << prefix << "<Symbol>" << m_name << "</Symbol>";
        };
    };
}
