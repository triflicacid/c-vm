#pragma once

#include <stack>
#include "lexer/Token.hpp"
#include "util/messages/list.hpp"
#include "Scope.hpp"
#include "Source.hpp"
#include "messages/MessageWithSource.hpp"
#include "LanguageOptions.hpp"

namespace language::parser {
    class Parser {
    private:
        int m_pos;
        std::stack<int> m_pos_stack; // Store position history
        Source *m_source;
        Scope *m_scope;

        /** Check if token exists at (pos+n). Default n=1. */
        bool exists(int n = 0) {
            return m_pos + n < m_source->tokens.size();
        }

        /** Get token at (pos+n). Default n=0. */
        lexer::Token *peek(int n = 0) {
            return &m_source->tokens[m_pos + n];
        }

        /** Increment position. */
        void move(int n = 1) { m_pos += n; }

        /** Set position. */
        void set(int pos) { m_pos = pos; }

        /** If we see the given token, return true. Increment position.
         * If provided, add error to message list [default nullptr]. */
        bool expect(lexer::Token::Type type, message::List *messages = nullptr);

        /** If we see one of the given tokens, return true. Increment position.
         * If provided, add error to message list [default nullptr]. */
        bool expect(const std::vector<lexer::Token::Type>& types, message::List *messages = nullptr);

        /** Same as expect(), but uses function as test. */
        bool expect(const std::function<bool(lexer::Token::Type)>& test_fn);

        /** Save current position. */
        void save();

        /** Remove position from stack (keep current position). */
        void discard();

        /** Restore old position. */
        void restore();

        /** Consume "decl ...". */
        bool consume_kw_decl(message::List &messages);

        /** Consume "data ...". */
        bool consume_kw_data(message::List &messages);

        /** Generate generic message with provided message about the current token. */
        message::MessageWithSource *generate_message(message::Level level, const std::string& message);

        /** Generate generic error with provided message about the current token. */
        message::MessageWithSource *generate_error(const std::string& message) {
            return generate_message(message::Level::Error, message);
        }

        /** Generate error on the given token. */
        message::MessageWithSource *generate_syntax_error(const std::vector<lexer::Token::Type>& expected);

    public:
        LanguageOptions options;

        explicit Parser(Source *source) : m_source(source), m_pos(0), m_scope(nullptr) {};

        [[nodiscard]] Scope *scope() const { return m_scope; }

        void set_scope(Scope *scope) { m_scope = scope; }

        void reset() {
            m_pos = 0;
            while (!m_pos_stack.empty()) m_pos_stack.pop();
        }

        /** Parse tokens. Provide error list and global scope to populate. */
        void parse(message::List& messages);
    };
}
