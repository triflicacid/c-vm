#pragma once

#include "parser/SymbolDeclaration.hpp"

namespace language::types {
    class UserType : public Type {
    private:
        int m_pos;
        const lexer::Token *m_token;
        int m_offset;
        std::map<std::string, const parser::SymbolDeclaration *> m_members;

    public:
        UserType(lexer::Token *token, int token_pos) : Type(), m_pos(token_pos), m_token(token), m_offset(0) { };

        ~UserType() {
            for (auto& pair : m_members) {
                delete pair.second;
            }
        }

        /** Get number of members. */
        [[nodiscard]] size_t member_count() const { return m_members.size(); }

        /** Check if member already exists. */
        [[nodiscard]] bool exists(const std::string& name) const;

        /** Get member with the given name */
        [[nodiscard]] const parser::SymbolDeclaration *get(const std::string& name) const;

        /** Add member (assumption: member does not exist already) */
        void add(parser::SymbolDeclaration *symbol);

        [[nodiscard]] int position() const { return m_pos; }

        [[nodiscard]] std::string name() const { return m_token->image(); }

        [[nodiscard]] size_t size() const override { return m_offset; }

        [[nodiscard]] Category category() const override { return Category::User; }

        [[nodiscard]] bool is_single_instance() const override { return true; }

        [[nodiscard]] std::string repr() const override { return m_token->image(); }

        void debug_print(std::ostream& stream, const std::string& prefix) const override;
    };
}
