#pragma once

#include <ostream>

namespace language::statement {
    enum class Type {
        GENERIC = -1,
        FUNCTION,
        RETURN,
        EXPRESSION,
        SYMBOL,
        NUMBER,
        BINARY_OPERATOR,
        UNARY_OPERATOR
    };

    class Statement {
    private:
        int m_pos;

    public:
        explicit Statement(int pos) : m_pos(pos) {};

        [[nodiscard]] virtual Type type() const { return Type::GENERIC; }

        [[nodiscard]] int position() const { return m_pos; }

        virtual void debug_print(std::ostream& stream, const std::string& prefix) const {
            stream << prefix << "<Statement />";
        }
    };
}
