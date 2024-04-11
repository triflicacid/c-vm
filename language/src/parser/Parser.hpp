#pragma once

#include <stack>
#include "util/messages/list.hpp"
#include "messages/MessageWithSource.hpp"
#include "ScopeManager.hpp"
#include "Program.hpp"
#include "statement/OperatorType.hpp"

namespace language::parser {
    class Parser {
    private:
        int m_pos; // Current index into token vector
        int m_end_pos; // Used in few cases, used to select a range. m_pos <= m_end_pos when used.
        std::stack<int> m_pos_stack; // Store position history
        Program *m_prog;
        std::vector<lexer::Token>& m_tokens;
        ScopeManager m_scopes;
        std::string m_entry_name; // Name of custom entry point
        const types::FunctionType *m_entry_type; // Type of entrypoint (if null, search for one).
        int m_entry_pos; // Position where "entry" was defined

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

        /** Mark m_pos as the end of a range. */
        void mark_end() { m_end_pos = m_pos; }

        /** Set end of range */
        void set_end(int pos) { m_end_pos = pos; }

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

        /** Consume "return ...". If asked, check return type against current function. */
        bool consume_kw_return(message::List &messages, statement::StatementBlock& block, bool check_return_type = false);

        /** Consume "entry ...". Populate program entries. */
        bool consume_kw_entry(message::List &messages);

        /** Declaration: check if identifier exists in topmost scope. If it does, add messages to stack. */
        bool check_can_create_identifier(const lexer::Token& identifier, int pos, message::List& messages);

        /** Declaration: check if we can shadow the given identifier. If not, add messages to stack. */
        bool check_can_shadow_identifier(const lexer::Token& identifier, int pos, message::List& messages);

        /** Check whether the given function overload already exists. */
        bool check_can_create_overload(const std::string& name, const types::FunctionType *overload, message::List& messages);

        /** Return if type `a` matches type `b` (order is important). If not, add messages to list. */
        bool check_type_match(const types::Type *a, const types::Type *b, int pos = 0, message::List *messages = nullptr);

        /** Check if a symbol is used: options.unused_symbol_level, add to messages. */
        bool check_symbol_unused(const parser::SymbolDeclaration *symbol, message::List& messages);

        /** Check that m_entry exists. */
        bool check_entry_point_exists(message::List& messages);

        /** Parse a function type. If provided, set type.position() to the given value.  */
        types::FunctionType *parse_function_type(message::List &messages, bool arg_list_required, int pos = -1);

        /** Parse an expression. */
        const statement::Expression *parse_expression(message::List& messages, int precedence = 0);

        /** Parse code block. Assume token "{" has already been eaten. Return pointer to block, please delete even if error. */
        statement::StatementBlock *parse_code_block(message::List& messages);

        /** Generate generic message with provided message about the current token. */
        message::MessageWithSource *generate_message(message::Level level, const std::string& message);

        /** Generate generic error with provided message about the current token. */
        message::MessageWithSource *generate_error(const std::string& message) {
            return generate_message(message::Level::Error, message);
        }

        /** Generate error on the given token. */
        message::MessageWithSource *generate_syntax_error(const std::vector<lexer::Token::Type>& expected);

        message::MessageWithSource *generate_syntax_error_multi(const std::vector<const std::vector<lexer::Token::Type> *>& expected);

        /** Generate error on the given token. */
        message::MessageWithSource *generate_custom_syntax_error(const std::string& expected);

    public:
        explicit Parser(Program *program) : m_prog(program), m_pos(0), m_end_pos(-1), m_tokens(program->source()->tokens), m_entry_pos(-1), m_entry_type(nullptr) {
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

    struct OperatorInfo {
        int precedence;
        bool right_associative;
        statement::OperatorType type;
    };

    extern std::map<lexer::Token::Type, OperatorInfo> token_binary_operators;
    extern std::map<lexer::Token::Type, OperatorInfo> token_unary_operators;
}
