#pragma once

#include "Expression.hpp"
#include "OperatorType.hpp"

namespace language::statement {
    class BinaryOperator : public Expression {
    private:
        OperatorType m_op;
        const Expression *m_lhs;
        const Expression *m_rhs;

    public:
        BinaryOperator(int pos, const lexer::Token *token, OperatorType op_type, const Expression *lhs, const Expression *rhs)
        : Expression(pos, token), m_op(op_type), m_lhs(lhs), m_rhs(rhs) {};

        ~BinaryOperator() {
            delete m_lhs;
            delete m_rhs;
        }

        [[nodiscard]] statement::Type type() const override { return statement::Type::BINARY_OPERATOR; }

        [[nodiscard]] const types::Type *get_type_of() const override {
            auto lhs = m_lhs->get_type_of(), rhs = m_rhs->get_type_of();
            return lhs->size() > rhs->size() ? lhs : rhs;
        }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            stream << prefix << "<BinaryOperator type=\"" << m_token->image() << "\">" << std::endl;
            m_lhs->debug_print(stream, prefix + "  ");
            stream << std::endl;
            m_rhs->debug_print(stream, prefix + "  ");
            stream << std::endl << prefix << "</BinaryOperator>";
        }
    };
}
