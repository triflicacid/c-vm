#include <sstream>
#include "FunctionType.hpp"

namespace language::types {
    bool FunctionType::equal(const FunctionType& other) const {
        if (argc() != other.argc()) {
            return false;
        }

        for (int i = 0; i < argc(); i++) {
            if (m_args[i]->repr() != other.m_args[i]->repr()) {
                return false;
            }
        }

        if (m_ret == nullptr) {
            return other.m_ret == nullptr;
        } else if (other.m_ret == nullptr) {
            return false;
        } else {
            return m_ret->repr() == other.m_ret->repr();
        }
    }

    void FunctionType::debug_print(std::ostream& stream, const std::string& prefix) const {
        stream << prefix << "<FunctionType id=\"" << m_id << "\" argc=\"" << argc() << "\">" << std::endl;

        if (m_args.empty()) {
            stream << prefix << "  <Arguments />" << std::endl;
        } else {
            stream << prefix << "  <Arguments>" << std::endl;

            for (auto &arg: m_args) {
                if (arg->category() == types::Type::Category::User) {
                    stream << prefix << "    <UserType>" << arg->repr() << "</UserType>";
                } else {
                    arg->debug_print(stream, prefix + "    ");
                }

                stream << std::endl;
            }

            stream << prefix << "  </Arguments>" << std::endl;
        }

        if (m_ret != nullptr) {
            stream << prefix << "  <Returns>" << std::endl;

            if (m_ret->category() == types::Type::Category::User) {
                stream << prefix << "    <UserType>" << m_ret->repr() << "</UserType>";
            } else {
                m_ret->debug_print(stream, prefix + "    ");
            }

            stream << std::endl << prefix << "  </Returns>" << std::endl;
        } else {
            stream << prefix << "  <Returns />" << std::endl;
        }

        stream << prefix << "</FunctionType>";
    }

    std::string FunctionType::repr() const {
        std::stringstream stream;
        stream << "(";

        for (int i = 0; i < m_args.size(); i++) {
            stream << m_args[i]->repr();

            if (i < m_args.size() - 1) stream << ", ";
        }

        stream << ") -> " << (m_ret ? m_ret->repr() : "()");

        return stream.str();
    }

    bool FunctionType::valid_entry_point(const FunctionType *type) {
        for (auto& arg : type->m_args) {
            if (arg->category() != Category::Numeric) {
                return false;
            }
        }

        if (type->m_ret && type->m_ret->category() != Category::Numeric) {
            return false;
        }

        return true;
    }
}
