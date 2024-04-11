#pragma once

#include "types/Type.hpp"

namespace language::parser {
    class SymbolDeclaration {
    private:
        int m_pos;
        std::string m_name;
        int m_offset;
        const types::Type *m_type;

    public:
        bool delete_type;
        bool was_assigned;
        bool was_used_since_assignment;
        int last_assigned_pos;

        SymbolDeclaration(int pos, const std::string &name, const types::Type *type)
        : m_type(type), m_pos(pos), m_name(name), m_offset(-1), delete_type(true), was_assigned(false),
        was_used_since_assignment(false), last_assigned_pos(-1) {};

        ~SymbolDeclaration() {
            if (!m_type->is_single_instance() && delete_type)
                delete m_type;
        }

        [[nodiscard]] std::string name() const { return m_name; }

        [[nodiscard]] const types::Type *type() const { return m_type; }

        [[nodiscard]] size_t size() const { return m_type->size(); }

        [[nodiscard]] int position() const { return m_pos; }

        [[nodiscard]] int offset() const { return m_offset; }

        void set_offset(int o) { m_offset = o; }

        void debug_print(std::ostream& stream, const std::string& prefix) const;
    };
}
