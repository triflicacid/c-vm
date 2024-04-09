#pragma once

#include "Type.hpp"
#include "parser/Symbol.hpp"
#include <ostream>

namespace language::types {
    class UserType : public Type {
    private:
        int m_offset;
        std::map<std::string, std::pair<const parser::Symbol *, int>> m_members;

    public:
        UserType(lexer::Token *token, int token_pos)
        : Type(token, token_pos), m_offset(0) { };

        ~UserType() {
            for (auto& pair : m_members) {
                delete pair.second.first;
            }
        }

        /** Check if member already exists. */
        [[nodiscard]] bool exists(const std::string& name) const;

        /** Get member with the given name */
        [[nodiscard]] const std::pair<const parser::Symbol *, int> *get(const std::string& name) const;

        /** Add member (assumption: member does not exist already) */
        void add(const parser::Symbol *symbol);

        [[nodiscard]] std::string name() const { return m_token->image(); }

        [[nodiscard]] size_t members() const { return m_members.size(); }

        [[nodiscard]] size_t size() const override { return m_offset; }

        [[nodiscard]] Category category() const override { return Category::User; }

        void debug_print(std::ostream& stream, const std::string& prefix) const override;
    };
}
