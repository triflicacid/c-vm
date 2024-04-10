#pragma once

#include "lexer/Token.hpp"

namespace language::types {
    class Type {
    public:
        enum class Category {
            None,
            Numeric,
            User,
            Function,
        };

    private:
        int m_pos;

    protected:
        lexer::Token *m_token;

    public:
        Type(lexer::Token *token, int token_pos) : m_token(token), m_pos(token_pos) {};

        [[nodiscard]] lexer::Token *token() const { return m_token; }

        [[nodiscard]] virtual size_t size() const { return 0; }

        [[nodiscard]] virtual Category category() const { return Category::None; }

        [[nodiscard]] int position() const { return m_pos; }

        /** Return if there should only be a single instance of this specific type. Used to know if we should delete it. */
        [[nodiscard]] virtual bool is_single_instance() const { return false; }

        virtual void debug_print(std::ostream& stream, const std::string& prefix) const {};
    };
}
