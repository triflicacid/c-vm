#include "UserType.hpp"
#include <ostream>

namespace language::types {
    bool UserType::exists(const std::string& name) const {
        return m_members.find(name) != m_members.end();
    }

    const parser::SymbolDeclaration *UserType::get(const std::string& name) const {
        auto entry = m_members.find(name);
        return entry == m_members.end() ? nullptr : entry->second;
    }

    void UserType::add(parser::SymbolDeclaration *symbol) {
        symbol->set_offset(m_offset);
        m_members.insert({ symbol->name(), symbol });

        m_offset += (int) symbol->size();
    }

    void UserType::debug_print(std::ostream& stream, const std::string& prefix) const {
        stream << prefix << "<Data size=\"" << size() << "\">" << std::endl;
        stream << prefix << "  <Name>" << name() << "</Name>" << std::endl << prefix << "  <InstanceMembers>" << std::endl;

        for (auto& pair : m_members) {
            pair.second->debug_print(stream, prefix + "    ");
            stream << std::endl;
        }

        stream << prefix << "  </InstanceMembers>" << std::endl;
        stream << prefix << "</Data>";
    }
}
