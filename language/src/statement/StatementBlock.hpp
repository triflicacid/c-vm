#pragma once

#include "Statement.hpp"
#include <vector>

namespace language::statement {
    class StatementBlock {
    private:
        int m_pos;
        std::vector<const Statement *> m_stmts;

    public:
        explicit StatementBlock(int pos) : m_pos(pos) {};

        ~StatementBlock() {
            for (auto& stmt : m_stmts) {
                delete stmt;
            }
        }

        [[nodiscard]] int position() const { return m_pos; }

        [[nodiscard]] size_t size() const { return m_stmts.size(); }

        void add(const Statement *stmt) {
            m_stmts.push_back(stmt);
        }

        [[nodiscard]] const std::vector<const Statement *> *statements() const { return &m_stmts; }

        void debug_print(std::ostream& stream, const std::string& prefix) const {
            if (m_stmts.empty()) {
                stream << prefix << "<StatementBlock />";
                return;
            }

            stream << prefix << "<StatementBlock>" << std::endl;

            for (auto& stmt : m_stmts) {
                stmt->debug_print(stream, prefix + "  ");
                stream << std::endl;
            }

            stream << prefix << "</StatementBlock>";
        }
    };
}
