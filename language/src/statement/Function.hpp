#pragma once

#include <utility>

#include "types/FunctionType.hpp"
#include "statement/Statement.hpp"

namespace language::statement {
    class Function : public Statement {
    private:
        std::string m_name;
        int m_id;
        const types::FunctionType *m_type;
        bool m_is_complete; // Do we have a complete definition?

        std::vector<std::string> m_params; // Parameter names
        std::vector<const statement::Statement *> m_body;

    public:
        Function(types::FunctionType *type, int id)
        : Statement(type->position()), m_name(type->token()->image()), m_type(type), m_id(id), m_is_complete(false) {
            type->set_id(id);
            type->is_used_by_fn = true;
        };

        ~Function() {
            delete m_type;

            for (auto& stmt : m_body) {
                delete stmt;
            }
        }

        [[nodiscard]] std::string name() const { return m_name; }

        [[nodiscard]] const types::Type *function_type() const { return m_type; }

        [[nodiscard]] virtual size_t size() const { return m_type->size(); }

        [[nodiscard]] int id() const { return m_id; }

        [[nodiscard]] int is_complete() const { return m_is_complete; }

        /** Complete function definition. Assume: !is_complete(). */
        void complete_definition(const std::vector<std::string>& params, const std::vector<const statement::Statement *>& body);

        [[nodiscard]] virtual bool can_be_assigned() const { return true; }

        void debug_print(std::ostream& stream, const std::string& prefix) const override;
    };
}
