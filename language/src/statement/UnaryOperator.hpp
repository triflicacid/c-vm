#pragma once

#include "Expression.hpp"
#include "OperatorType.hpp"

namespace language::statement {
    class UnaryOperator : public Expression {
    private:
        OperatorType m_op;
        const Expression *m_data;

    public:
        UnaryOperator(int pos, const lexer::Token *token, OperatorType op_type, const Expression *data)
        : Expression(pos, token), m_op(op_type), m_data(data) {};

        ~UnaryOperator() {
            delete m_data;
        }

        [[nodiscard]] statement::Type type() const override { return statement::Type::UNARY_OPERATOR; }

        [[nodiscard]] const types::Type *get_type_of() const override { return m_data->get_type_of(); }

        void debug_print(std::ostream& stream, const std::string& prefix) const override {
            stream << prefix << "<UnaryOperator type=\"" << m_token->image() << "\">" << std::endl;
            m_data->debug_print(stream, prefix + "  ");
            stream << std::endl << prefix << "</UnaryOperator>";
        }
    };
}
