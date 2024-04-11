#include "Function.hpp"

namespace language::statement {
    Function::Function(const std::string& name, types::FunctionType *type, int id)
            : Statement(type->position()), m_name(name), m_type(type), m_body(nullptr), m_id(id), m_is_complete(false) {
        type->set_id(id);
        type->is_used_by_fn = true;
    };

    void Function::debug_print(std::ostream &stream, const std::string &prefix) const  {
        stream << prefix << "<Function id=\"" << m_id << "\" complete=\"" << m_is_complete << "\">" << std::endl;
        stream << prefix << "  <Name>" << m_name << "</Name>" << std::endl;
        m_type->debug_print(stream, prefix + "  ");
        stream << std::endl;

        if (m_is_complete) {
            stream << prefix << "  <Parameters>" << std::endl;
            auto offsets = m_type->arg_offsets();

            for (int i = 0; i < offsets.size(); i++) {
                stream << prefix << "    <Parameter offset=\"" << offsets[i] << "\">" << m_params[i].first << "</Parameter>" << std::endl;
            }

            stream << prefix << "  </Parameters>" << std::endl;
            stream << prefix << "  <Body>" << std::endl;
            m_body->debug_print(stream, prefix + "    ");
            stream << std::endl << prefix << "  </Body>" << std::endl;
        }

        stream << prefix << "</Function>";
    }

    void Function::set_params(const std::vector<std::pair<std::string, int>> &params) {
        m_params = params;
    }

    void Function::set_body(const StatementBlock *body) {
        if (m_is_complete)
            return;

        m_body = body;
        m_is_complete = true;
    }

    void Function::add_args_to_scope(parser::Scope *scope) const {
        auto offsets = m_type->arg_offsets();

        for (int i = 0; i < m_params.size(); i++) {
            // IMPORTANT indicate as argument to prevent offset overwrite
            auto *symbol = new parser::SymbolDeclaration(m_params[i].second, m_params[i].first, m_type->arg(i), true);
            symbol->set_offset(offsets[i]);
            scope->var_create(symbol);
        }
    }
}
