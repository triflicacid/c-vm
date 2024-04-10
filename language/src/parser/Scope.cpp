#include "Scope.hpp"
#include <ostream>

namespace language::parser {
    void Scope::clear() {
        m_offset = m_size = 0;
        m_symbols.clear();
    }

    bool Scope::var_exists(const std::string &name) {
        return m_symbols.find(name) != m_symbols.end();
    }

    const std::pair<const Symbol *, int> *Scope::var_get(const std::string &name) {
        auto found = m_symbols.find(name);
        return found == m_symbols.end() ? nullptr : &found->second;
    }

    const Symbol *Scope::var_create(const Symbol *symbol) {
        auto entry = m_symbols.find(symbol->name());
        const Symbol *old_symbol;

        if (entry == m_symbols.end()) {
            m_symbols.insert({ symbol->name(), { symbol, (int) m_offset } });
            old_symbol = nullptr;
        } else {
            old_symbol = entry->second.first;
            entry->second.first = symbol;
            entry->second.second = (int) m_offset;
        }

        grow(symbol->size());
        return old_symbol;
    }

    bool Scope::data_exists(const std::string &name) {
        return m_user_types.find(name) != m_user_types.end();
    }

    const types::UserType *Scope::data_get(const std::string &name) {
        auto entry = m_user_types.find(name);
        return entry == m_user_types.end() ? nullptr : entry->second;
    }

    void Scope::data_create(const types::UserType *data) {
        m_user_types.insert({ data->name(), data });
    }

    void Scope::debug_print(std::ostream &stream, const std::string& prefix) {
        stream << prefix << "<Scope size=\"" << m_size << "\" symbols=\"" << m_symbols.size() << "\">" << std::endl;

        if (!m_symbols.empty()) {
            stream << prefix << "  <Symbols>" << std::endl;

            for (auto &pair : m_symbols) {
                pair.second.first->debug_print(stream, pair.second.second, prefix + "    ");
                stream << std::endl;
            }

            stream << prefix << "  </Symbols>" << std::endl;
        }

        if (!m_user_types.empty()) {
            stream << prefix << "  <UserTypes>" << std::endl;

            for (auto &pair : m_user_types) {
                pair.second->debug_print(stream, prefix + "    ");
                stream << std::endl;
            }

            stream << prefix << "  </UserTypes>" << std::endl;
        }

        stream << prefix << "</Scope>";
    }

    bool Scope::func_exists(const std::string &name) {
        return m_functions.find(name) != m_functions.end();
    }

    bool Scope::func_exists(const std::string &name, const types::FunctionType *overload) {
        auto entry = m_functions.find(name);

        if (entry == m_functions.end()) {
            return false;
        }

        for (auto& f_type : entry->second) {
            if (f_type->equal(*overload)) {
                return true;
            }
        }

        return false;
    }

    const std::vector<const types::FunctionType *> *Scope::func_get(const std::string &name) {
        auto pair = m_functions.find(name);
        return pair == m_functions.end() ? nullptr : &pair->second;
    }

    const types::FunctionType *Scope::func_get(const std::string &name, const types::FunctionType *overload) const {
        auto vec = m_functions.find(name);

        if (vec == m_functions.end()) {
            return nullptr;
        }

        for (auto& f_type : vec->second) {
            if (f_type->equal(*overload)) {
                return f_type;
            }
        }

        return nullptr;
    }

    void Scope::func_create(const std::string& name, const types::FunctionType *overload) {
        auto entry = m_functions.find(name);

        if (entry == m_functions.end()) {
            m_functions.insert({ name, { overload } });
        } else {
            entry->second.push_back(overload);
        }
    }
}
