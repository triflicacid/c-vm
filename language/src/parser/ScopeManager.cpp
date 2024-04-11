#include "ScopeManager.hpp"

namespace language::parser {

    void ScopeManager::push(int invoker_func_id) {
        m_scopes.push_back(new Scope(invoker_func_id));
    }

    void ScopeManager::push(Scope *scope) {
        m_scopes.push_back(scope);
    }

    void ScopeManager::pop() {
        if (!m_scopes.empty())
            m_scopes.pop_back();
    }

    bool ScopeManager::var_exists(const std::string &name) const {
        for (int i = (int) m_scopes.size() - 1; i >= 0; i--) {
            if (m_scopes[i]->var_exists(name)) {
                return true;
            }
        }

        return false;
    }

    parser::SymbolDeclaration *ScopeManager::var_get(const std::string &name) const {
        for (int i = (int) m_scopes.size() - 1; i >= 0; i--) {
            if (m_scopes[i]->var_exists(name)) {
                return m_scopes[i]->var_get(name);
            }
        }

        return nullptr;
    }

    bool ScopeManager::data_exists(const std::string &name) const {
        for (int i = (int) m_scopes.size() - 1; i >= 0; i--) {
            if (m_scopes[i]->data_exists(name)) {
                return true;
            }
        }

        return false;
    }

    const types::UserType *ScopeManager::data_get(const std::string &name) const {
        for (int i = (int) m_scopes.size() - 1; i >= 0; i--) {
            if (m_scopes[i]->data_exists(name)) {
                return m_scopes[i]->data_get(name);
            }
        }

        return nullptr;
    }

    void ScopeManager::debug_print(std::ostream &stream, const std::string &prefix) const {
        stream << prefix << "<Stack>" << std::endl;

        for (auto &scope : m_scopes) {
            scope->debug_print(stream, prefix + "  ");
            stream << std::endl;
        }

        stream << prefix << "</Stack>";
    }

    bool ScopeManager::func_exists(const std::string &name) const {
        for (int i = (int) m_scopes.size() - 1; i >= 0; i--) {
            if (m_scopes[i]->func_exists(name)) {
                return true;
            }
        }

        return false;
    }

    bool ScopeManager::func_exists(const std::string &name, const types::FunctionType *overload) const {
        for (int i = (int) m_scopes.size() - 1; i >= 0; i--) {
            if (m_scopes[i]->func_exists(name, overload)) {
                return true;
            }
        }

        return false;
    }

    const std::vector<const types::FunctionType *> *ScopeManager::func_get(const std::string &name) const {
        for (int i = (int) m_scopes.size() - 1; i >= 0; i--) {
            if (m_scopes[i]->func_exists(name)) {
                return m_scopes[i]->func_get(name);
            }
        }

        return nullptr;
    }

    const types::FunctionType *
    ScopeManager::func_get(const std::string &name, const types::FunctionType *overload) const {
        const types::FunctionType *match;

        for (int i = (int) m_scopes.size() - 1; i >= 0; i--) {
            match = m_scopes[i]->func_get(name, overload);

            if (match != nullptr) {
                return match;
            }
        }

        return nullptr;
    }
}
