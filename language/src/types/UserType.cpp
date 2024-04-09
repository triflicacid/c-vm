#include "UserType.hpp"

namespace language::types {
    bool UserType::exists(const std::string& name) const {
        return m_members.find(name) != m_members.end();
    }

    const std::pair<const parser::Symbol *, int> *UserType::get(const std::string& name) const {
        auto entry = m_members.find(name);
        return entry == m_members.end() ? nullptr : &entry->second;
    }

    void UserType::add(const parser::Symbol *symbol) {
        bool physical = symbol->type() == parser::Symbol::Type::Variable;

        m_members.insert({ symbol->name(), { symbol, physical ? m_offset : -1 } });

        if (physical) {
            m_offset += (int) symbol->size();
        }
    }

    void UserType::debug_print(std::ostream& stream, const std::string& prefix) const {
        stream << prefix << "<Data size=\"" << size() << "\">" << std::endl;
        stream << prefix << "  <Name>" << name() << "</Name>" << std::endl << prefix << "  <InstanceMembers>" << std::endl;

        int count = 0;

        for (auto& pair : m_members) {
            if (pair.second.second > -1) {
                pair.second.first->debug_print(stream, pair.second.second, prefix + "    ");
                stream << std::endl;
            } else {
                count++;
            }
        }

        stream << prefix << "  </InstanceMembers>" << std::endl;

        if (count > 0) {
            stream << prefix << "  <StaticMembers>" << std::endl;

            for (auto& pair : m_members) {
                if (pair.second.second < 0) {
                    pair.second.first->debug_print(stream, pair.second.second, prefix + "    ");
                    stream << std::endl;
                }
            }

            stream << prefix << "  </StaticMembers>" << std::endl;
        }

        stream << prefix << "</Data>";
    }
}
