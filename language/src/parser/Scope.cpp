#include "Scope.hpp"
#include <ostream>

namespace language::parser {
    void Scope::clear() {
        m_offset = m_size = 0;
        m_symbols.clear();
    }

    bool Scope::exists(const std::string &name) {
        return m_symbols.find(name) != m_symbols.end();
    }

    const std::pair<const Symbol *, int> *Scope::get(const std::string &name) {
        auto found = m_symbols.find(name);
        return found == m_symbols.end() ? nullptr : &found->second;
    }

    const Symbol *Scope::create(const Symbol *symbol, bool has_size) {
        auto entry = m_symbols.find(symbol->name());
        const Symbol *old_symbol;
        int offset = has_size ? (int) m_offset : 0;

        if (entry == m_symbols.end()) {
            m_symbols.insert({ symbol->name(), { symbol, offset } });
            old_symbol = nullptr;
        } else {
            old_symbol = entry->second.first;
            entry->second.first = symbol;
            entry->second.second = offset;
        }

        if (has_size) grow(symbol->size());
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
}
