#pragma once

#include "lexer/Token.hpp"
#include "types/Type.hpp"

namespace language::parser {
    class Symbol {
    private:
        int m_pos;

    protected:
        std::string m_name;
        const types::Type *m_type;

    public:
        Symbol(int pos, const std::string& name, const types::Type *type)
        : m_pos(pos), m_name(name), m_type(type) {};

        ~Symbol() {
            if (!m_type->is_single_instance())
                delete m_type;
        }

        [[nodiscard]] std::string name() const { return m_name; }

        [[nodiscard]] int position() const { return m_pos; }

        [[nodiscard]] const types::Type *type() const { return m_type; }

        [[nodiscard]] size_t size() const { return m_type->size(); }

        void debug_print(std::ostream& stream, size_t offset, const std::string& prefix) const;
    };
}
