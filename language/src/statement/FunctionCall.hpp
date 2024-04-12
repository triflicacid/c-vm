#pragma once

#include "Function.hpp"
#include "FunctionRef.hpp"

namespace language::statement {
    class FunctionCall : public Expression {
    private:
        const statement::Function *m_func;
        std::vector<const statement::Expression *> m_args;

    public:
        FunctionCall(int pos, const lexer::Token *token, const statement::Function *func, const std::vector<const statement::Expression *>& args)
        : Expression(pos, token), m_func(func), m_args(args) {};

        /** Extrapolate call from a reference. Note, reference MUST have a type. */
        FunctionCall(const statement::FunctionRef *ref, const std::vector<const statement::Expression *>& args)
                : Expression(ref->position(), ref->token()), m_func(ref->function()), m_args(args) {};

        ~FunctionCall() {
            for (auto& arg : m_args) {
                delete arg;
            }
        }

        [[nodiscard]] Type type() const override { return Type::FUNCTION_CALL; }

        [[nodiscard]] const statement::Function *function() const { return m_func; }

        [[nodiscard]] const types::Type *get_type_of() const override { return m_func->function_type()->return_type(); }

        void debug_print(std::ostream& stream, const std::string& prefix) const override;
    };
}
