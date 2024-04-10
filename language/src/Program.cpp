#include "Program.hpp"

namespace language {
    void Program::debug_print(std::ostream &stream, const std::string &prefix) {
        stream << prefix << "<Program>" << std::endl;
        m_scope->debug_print(stream, prefix + "  ");
        stream << std::endl << prefix << "  ";

        if (m_funcs.empty()) {
            stream << "<Functions />";
        } else {
            stream << "<Functions>" << std::endl;

            for (auto& func : m_funcs) {
                func.second->debug_print(stream, prefix + "    ");
                stream << std::endl;
            }

            stream << "  </Functions>";
        }

        stream << std::endl << prefix << "</Program>";
    }
}
