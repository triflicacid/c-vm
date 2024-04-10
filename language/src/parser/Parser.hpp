#pragma once

#include <stack>
#include "lexer/Token.hpp"
#include "util/messages/list.hpp"
#include "Scope.hpp"
#include "Source.hpp"
#include "messages/MessageWithSource.hpp"
#include "LanguageOptions.hpp"
#include "ScopeManager.hpp"
#include "Program.hpp"

namespace language::parser {
    class Parser {
    private:
        int m_pos;
        std::stack<int> m_pos_stack; // Store position history
        Program *m_prog;
        std::vector<lexer::Token>& m_tokens;
        ScopeManager m_scopes;

        /** Check if token var_exists at (pos+n). Default n=1. */
        bool exists(int n = 0) {
            return m_pos + n < m_tokens.size();
        }

        /** Get token at (pos+n). Default n=0. */
        lexer::Token *peek(int n = 0) {
            return &m_tokens[m_pos + n];
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

        /** Parse a type, populate type pointer. Return if success, increment m_pos. */
        bool consume_type(message::List& messages, const types::Type **type);

        /** Consume "decl func ...". */
        bool consume_kw_decl_func(message::List &messages);

        /** Consume "decl ...". */
        bool consume_kw_decl(message::List &messages);

        /** Consume "data ...". */
        bool consume_kw_data(message::List &messages);

        /** Consume "func ...". */
        bool consume_kw_func(message::List &messages);

        /** Declaration: check if identifier exists in topmost scope. If it does, add messages to stack. */
        bool check_can_create_identifier(const lexer::Token& identifier, int pos, message::List& messages);

        /** Declaration: check if we can shadow the given identifier. If not, add messages to stack. */
        bool check_can_shadow_identifier(const lexer::Token& identifier, int pos, message::List& messages);

        /** Check whether the given function overload already exists. */
        bool check_can_create_overload(const std::string& name, const types::FunctionType *overload, message::List& messages);

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

        explicit Parser(Program *program) : m_prog(program), m_pos(0), m_tokens(program->source()->tokens) {
            // TODO remove, no scopes should be preserved.
            m_scopes.push(program->global_scope());
            m_scopes.set_immortal(1);
        };

        void reset() {
            m_pos = 0;
            while (!m_pos_stack.empty()) m_pos_stack.pop();
        }

        [[nodiscard]] const ScopeManager *scopes() const { return &m_scopes; }

        /** Return: are we in the global scope? */
        [[nodiscard]] bool in_global_scope() const { return m_scopes.size() == 1; }

        /** Parse tokens. Provide error list and global scope to populate. */
        void parse(message::List& messages);
    };
}
