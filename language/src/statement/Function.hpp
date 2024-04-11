#pragma once

#include <utility>

#include "types/FunctionType.hpp"
#include "statement/Statement.hpp"
#include "StatementBlock.hpp"

namespace language::statement {
    class Function : public Statement {
    private:
        std::string m_name;
        int m_id;
        const types::FunctionType *m_type;
        bool m_is_complete; // Do we have a complete definition?

        std::vector<std::string> m_params; // Parameter names
        const StatementBlock *m_body;

    public:
        Function(types::FunctionType *type, int id)
        : Statement(type->position()), m_name(type->name()), m_type(type), m_body(nullptr), m_id(id), m_is_complete(false) {
            type->set_id(id);
            type->is_used_by_fn = true;
        };

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

        /** Complete function definition. Assume: !is_complete(). */
        void complete_definition(const std::vector<std::string>& params, const StatementBlock *body);

        void debug_print(std::ostream& stream, const std::string& prefix) const override;
    };
}
