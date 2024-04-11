#pragma once

#include "Scope.hpp"

namespace language::parser {
    class ScopeManager {
    private:
        std::vector<Scope *> m_scopes;
        int m_immortal; // All scopes < this index are immortal

    public:
        ScopeManager() : m_immortal(0) {};

        ~ScopeManager() {
            for (int i = m_immortal; i < m_scopes.size(); i++) {
                delete m_scopes[i];
            }
        }

        /** Set immortal number. */
        void set_immortal(int i) { m_immortal = i; }

        [[nodiscard]] size_t size() const { return m_scopes.size(); }

        /** Get topmost scope. */
        [[nodiscard]] Scope *get_local() const { return m_scopes.back(); }

        /** Add a new scope. */
        void push(int);

        /** Push the given scope. */
        void push(Scope *);

        /** Pop the topmost scope. */
        void pop();

        /** Query if symbol exists. */
        [[nodiscard]] bool var_exists(const std::string& name) const;

        /** Get symbol information. */
        [[nodiscard]] parser::SymbolDeclaration *var_get(const std::string& name) const;

        /** Query if data type var_exists. */
        [[nodiscard]] bool data_exists(const std::string& name) const;

        /** Get symbol information. */
        [[nodiscard]] const types::UserType *data_get(const std::string& name) const;

        /** Query if function name var_exists. */
        [[nodiscard]] bool func_exists(const std::string& name) const;

        /** Query if function overload name var_exists. */
        [[nodiscard]] bool func_exists(const std::string& name, const types::FunctionType *overload) const;

        /** Get function. */
        [[nodiscard]] const std::vector<const types::FunctionType *> *func_get(const std::string& name) const;

        [[nodiscard]] const types::FunctionType *func_get(const std::string& name, const types::FunctionType *overload) const;

        /** DEBUG: print all scopes to stream */
        void debug_print(std::ostream& stream, const std::string& prefix = "") const;
    };
}
