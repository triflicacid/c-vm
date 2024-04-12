#pragma once

#include "Expression.hpp"
#include "Function.hpp"

namespace language::statement {
    class FunctionRef : public Expression {
    public:
        std::string m_name;
        const statement::Function *m_func; // If nullptr, hasn't selected a signature

    public:
        FunctionRef(int pos, const lexer::Token *token, const std::string& name)
        : Expression(pos, token), m_name(name), m_func(nullptr) {};

        FunctionRef(int pos, const lexer::Token *token, const statement::Function *func)
                : Expression(pos, token), m_name(func->name()), m_func(func) {};

        [[nodiscard]] Type type() const override { return Type::FUNCTION_REF; }

        [[nodiscard]] std::string name() const { return m_name; }

        [[nodiscard]] const statement::Function *function() const { return m_func; }

        void set_function(const statement::Function *func) { m_func = func; }

        [[nodiscard]] const types::Type *get_type_of() const override { return m_func == nullptr ? nullptr : m_func->function_type(); }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            if (m_func == nullptr) {
                stream << prefix << "<UnresolvedFunction>" << m_name << "</UnresolvedFunction>";
            } else {
                m_func->debug_print(stream, prefix);
            }
        }
    };
}
