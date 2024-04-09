#pragma once

#include "Symbol.hpp"
#include "util/util.h"
#include "types/Type.hpp"

namespace language::parser {
    class FunctionSymbol : public Symbol {
    private:
        std::vector<types::Type> m_args; // Vector of argument type tokens
        types::Type m_ret; // Return type token

    public:
        FunctionSymbol(int pos, const std::string& name, const std::vector<types::Type>& args, const types::Type& ret)
        : Symbol(Type::Function, pos, name), m_args(args), m_ret(ret) {};

        [[nodiscard]] size_t size() const override { return sizeof(WORD_T); };

        /** Check if two signatures are equal */
        bool equal(const FunctionSymbol& other);

        /** Get argument count. */
        [[nodiscard]] size_t argc() const { return m_args.size(); }

        /** Get ith argument. */
        [[nodiscard]] types::Type arg(int i) const { return m_args[i]; }

        /** Get return type token. */
        [[nodiscard]] types::Type ret() const { return m_ret; }

        [[nodiscard]] bool can_be_assigned() const override { return false; }

        void debug_print(std::ostream& stream, size_t offset, const std::string& prefix) const override;
    };
}
