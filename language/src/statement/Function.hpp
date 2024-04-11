#pragma once

#include <utility>

#include "types/FunctionType.hpp"
#include "statement/Statement.hpp"
#include "StatementBlock.hpp"
#include "parser/Scope.hpp"

namespace language::statement {
    class Function : public Statement {
    private:
        std::string m_name;
        int m_id;
        const types::FunctionType *m_type;
        bool m_is_complete; // Do we have a complete definition?

        std::vector<std::pair<std::string, int>> m_params; // Parameter names, positions (token)
        const StatementBlock *m_body;

    public:
        Function(const std::string& name, types::FunctionType *type, int id);

        ~Function() {
            delete m_type;
            delete m_body;
        }

        [[nodiscard]] std::string name() const { return m_name; }

        [[nodiscard]] Type type() const override { return Type::FUNCTION; }

        [[nodiscard]] virtual size_t size() const { return m_type->size(); }

        [[nodiscard]] int id() const { return m_id; }

        [[nodiscard]] const types::FunctionType *function_type() const { return m_type; }

        [[nodiscard]] int is_complete() const { return m_is_complete; }

        /** Add parameters to a scope. */
        void add_args_to_scope(parser::Scope *scope) const;

        void set_params(const std::vector<std::pair<std::string, int>>& params);

        void set_body(const StatementBlock *body);

        void debug_print(std::ostream& stream, const std::string& prefix) const override;
    };
}
