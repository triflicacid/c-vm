#include "Symbol.hpp"
#include "types/FunctionType.hpp"

namespace language::parser {

    void Symbol::debug_print(std::ostream &stream, size_t offset, const std::string &prefix) const {
        stream << prefix << "<Variable offset=\"" << offset << "\" bytes=\"" << size() << "\">" << std::endl;

        if (m_type->category() == types::Type::Category::User) {
            // Don't both printing out internal info
            stream << prefix << "  <UserType>" << m_type->token()->image() << "</UserType>";
        } else if (m_type->category() == types::Type::Category::Function &&
                ((types::FunctionType *) m_type)->is_stored()) {
            // Don't both printing out internal info
            stream << prefix << "  <FunctionType id=\"" << ((types::FunctionType *) m_type)->id() << "\" />";
        } else {
            m_type->debug_print(stream, prefix + "  ");
        }

        stream << std::endl << prefix << "  <Name>" << m_name << "</Name>" << std::endl << prefix << "</Variable>";
    }
}
