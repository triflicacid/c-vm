#include "SymbolDeclaration.hpp"
#include "types/FunctionType.hpp"

namespace language::parser {

    void SymbolDeclaration::debug_print(std::ostream &stream, const std::string &prefix) const {
        std::string tag_name = m_is_arg ? "Argument" : "Variable";

        stream << prefix << "<" << tag_name << " offset=\"" << m_offset << "\" bytes=\"" << size() << "\">" << std::endl;

        if (m_type->category() == types::Category::User) {
            // Don't both printing out internal info
            stream << prefix << "  <UserType>" << m_type->repr() << "</UserType>";
        } else if (m_type->category() == types::Category::Function &&
                ((types::FunctionType *) m_type)->is_stored()) {
            // Don't both printing out internal info
            stream << prefix << "  <FunctionType id=\"" << ((types::FunctionType *) m_type)->id() << "\" />";
        } else {
            m_type->debug_print(stream, prefix + "  ");
        }

        stream << std::endl << prefix << "  <Name>" << name() << "</Name>" << std::endl << prefix << "</" << tag_name << ">";
    }
}
