#pragma once

#include "Statement.hpp"
#include "Expression.hpp"

namespace language::statement {
    class ReturnStatement : public Statement {
    private:
        const Expression *m_expr; // May be nullptr

    public:
        ReturnStatement(int pos, const Expression *expr) : Statement(pos), m_expr(expr) {};

        [[nodiscard]] Type type() const override { return Type::RETURN; }

        [[nodiscard]] bool is_empty() const { return m_expr == nullptr; }

        /** Get return type. */
        [[nodiscard]] const types::Type *get_type_of() const { return m_expr ? m_expr->get_type_of() : nullptr; }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            stream << prefix;

            if (m_expr == nullptr) {
                stream << "<Return />";
            } else {
                stream << "<Return>" << std::endl;
                m_expr->debug_print(stream, prefix + "  ");
                stream << std::endl << prefix << "</Return>";
            }
        }
    };
}
