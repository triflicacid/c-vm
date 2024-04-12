#include "FunctionCall.hpp"

namespace language::statement {

    void FunctionCall::debug_print(std::ostream &stream, const std::string &prefix) const {
        stream << prefix << "<FunctionCall id=\"" << m_func->id() << "\">" << std::endl;

        auto offsets = m_func->function_type()->arg_offsets();

        for (int i = 0; i < m_args.size(); i++) {
            stream << prefix << "  <Argument offset=\"" << offsets[i] << "\">" << std::endl;
            m_args[i]->debug_print(stream, prefix + "    ");
            stream << std::endl << prefix << "  </Argument>" << std::endl;
        }

        stream << prefix << "</FunctionCall>";
    }
}
