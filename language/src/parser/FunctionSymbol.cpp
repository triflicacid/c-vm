#include "FunctionSymbol.hpp"

namespace language::parser {
    bool FunctionSymbol::equal(const FunctionSymbol& other) {
        if (m_name != other.m_name || argc() != other.argc()) {
            return false;
        }

        for (int i = 0; i < argc(); i++) {
            if (m_args[i].token()->image() != other.m_args[i].token()->image()) {
                return false;
            }
        }

        return true;
    }

    void FunctionSymbol::debug_print(std::ostream& stream, size_t offset, const std::string& prefix) const {
        stream << prefix << "<Function offset=\"" << offset << "\">" << std::endl;
        stream << prefix << "  <Name>" << m_name << "</Name>" << std::endl;
        stream << prefix << "  <Arguments>" << std::endl;

        for (auto& arg : m_args) {
            arg.debug_print(stream, prefix + "    ");
            stream << std::endl;
        }

        stream << prefix << "  </Arguments>" << std::endl << prefix << "  <Returns>";
        m_ret.debug_print(stream, prefix + "    ");
        stream << std::endl << prefix << "  </Returns>" << std::endl;
        stream << prefix << "</Function>";
    }
}
