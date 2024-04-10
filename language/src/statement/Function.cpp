#include "Function.hpp"

namespace language::statement {

    void Function::debug_print(std::ostream &stream, const std::string &prefix) const {
        stream << prefix << "<Function id=\"" << m_id << "\" complete=\"" << m_is_complete << "\">" << std::endl;
        stream << prefix << "  <Name>" << m_name << "</Name>" << std::endl;
        m_type->debug_print(stream, prefix + "  ");
        stream << std::endl;

        if (m_is_complete) {
            stream << prefix << "  <Body statements=\"" << m_body.size() << "\">" << std::endl;

            for (auto &stmt: m_body) {
                stmt->debug_print(stream, prefix + "    ");
                stream << std::endl;
            }

            stream << prefix << "  </Body>" << std::endl;
        }

        stream << prefix << "</Function>";
    }

    void Function::complete_definition(const std::vector<std::string> &params,
                                       const std::vector<const statement::Statement *> &body) {
        if (m_is_complete)
            return;

        m_params = params;
        m_body = body;
        m_is_complete = true;
    }
}
