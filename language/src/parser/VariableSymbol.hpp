#pragma once

#include "Symbol.hpp"

#include <utility>

#include "lexer/Token.hpp"
#include "util/util.h"
#include "types/Type.hpp"

namespace language::parser {
    class VariableSymbol : public Symbol {
    private:
        const types::Type *m_type;

    public:
        VariableSymbol(int pos, const std::string& name, const types::Type *type)
        : Symbol(Type::Variable, pos, name), m_type(type) {};

        ~VariableSymbol() {
            delete m_type;
        }

        [[nodiscard]] const types::Type *variable_type() const { return m_type; }

        [[nodiscard]] size_t size() const override { return m_type->size(); }

        [[nodiscard]] bool can_be_assigned() const override { return true; }

        void debug_print(std::ostream& stream, size_t offset, const std::string& prefix) const override {
            stream << prefix << "<Variable offset=\"" << offset << "\" bytes=\"" << size() << "\">" << std::endl;

            if (m_type->category() == types::Type::Category::User) {
                // Don't both printing out internal info
                stream << prefix << "  <UserType>" << m_type->token()->image() << "</UserType>";
            } else {
                m_type->debug_print(stream, prefix + "  ");
            }

            stream << std::endl << prefix << "  <Name>" << m_name << "</Name>" << std::endl << prefix << "</Variable>";
        }
    };
}
