#pragma once

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include "Message.h"
#include "tokens/Token.h"
#include "tokens/SymbolDecl.h"
#include <sstream>
#include "Options.h"
#include <map>
#include <iostream>

namespace language::parser {
    class SymbolScope {
    private:
        std::map<std::string, std::pair<tokens::SymbolDeclaration *, int>> symbols{}; // declaration of the associated symbol, offset
        int scope_offset = 0;

    public:
        SymbolScope() = default;

        explicit SymbolScope(int starting_offset) : scope_offset(starting_offset) { };

        /** Given a symbol, returns its declaration */
        tokens::SymbolDeclaration *get_decl(const std::string &symbol);

        /** Get offset of the given symbol, default -1 */
        int get_offset(const std::string &symbol);

        /** Insert symbol into the scope. Return its offset. */
        int insert_symbol(const std::string& symbol, tokens::SymbolDeclaration *declaration);

        /** Get size (bytes) of the scope */
        [[nodiscard]] int size() const {
            return scope_offset;
        }
    };

    class SourceData {
    private:
        std::string m_path;
        std::string m_contents;

        bool m_parsed = false;
        std::vector<tokens::Token *> m_tokens;

        // Map variables in each grammatical scope to the stack offset
        std::vector<SymbolScope> m_symbol_scopes;

        std::stringstream m_out;

    public:
        /** Create a source and its data */
        SourceData(std::string source, std::string contents) : m_path(std::move(source)), m_contents(std::move(contents)) {};

        /** Create an empty source - populate it in the future */
        explicit SourceData(std::string source) : m_path(std::move(source)) {};

        ~SourceData() {
            for (auto token : m_tokens) {
                delete token;
            }

            m_tokens.clear();
        }

        [[nodiscard]] std::stringstream *out() { return &m_out; }

        [[nodiscard]] std::string get_path() const { return m_path; }

        [[nodiscard]] std::string get_contents() const { return m_contents; }

        /** Has this file been parsed? */
        [[nodiscard]] bool was_parsed() const { return m_parsed; }

        /** Reset parsed data */
        void reset();

        /** Reset contents */
        void set_contents(std::string contents);

        /** Load content from file, return if OK */
        bool load_from_file();

        void reset_tokens();

        void add_token(tokens::Token *token) {
            m_tokens.push_back(token);
        }

        /** Return number of symbol scopes */
        [[nodiscard]] int count_symbol_scopes() const { return (int) m_symbol_scopes.size(); };

        /* Push a new symbol scope */
        int push_symbol_scope();

        /** Remove latest symbol scope */
        void pop_symbol_scope();

        /** Get the scope of a given symbol, or return -1 */
        [[nodiscard]] int get_symbol_scope(const std::string &symbol);

        /** Get offset of a symbol */
        [[nodiscard]] int get_symbol_offset(const std::string &symbol);

        /** Get declaration of a symbol */
        [[nodiscard]] tokens::SymbolDeclaration *get_symbol_declaration(const std::string &symbol);

        /** Create a new symbol in the latest scope */
        void insert_symbol(const std::string& symbol, tokens::SymbolDeclaration *declaration);

        // DEBUG
        void debug_print_tokens() {
            for (auto token : m_tokens) {
                token->debug_print();
            }
        }

        void build();
    };

    class Data {
        friend class Parser;
    private:
        std::map<std::string, SourceData> m_sources;

        Options m_options; // Parse/build options
        SourceData *m_sdata = nullptr; // Current SourceData being parsed
        std::string m_source; // Current source being parsed
        int m_row = 0; // Current row in parsing

        std::vector<Message> m_messages; // Messages accrued over parsing/building
        std::stringstream m_out_stream; // Build output stream

    public:
        Data() = default;

        ~Data() {
            clear_messages();
        }

        /** Add a source. Assume this is also its path. */
        void add_source(const std::string& source_name) {
            add_source(source_name, source_name);
        }

        /** Add a source */
        void add_source(const std::string& source_name, std::string source_path) {
            auto it = m_sources.find(source_name);
            if (it == m_sources.end()) {
                m_sources.insert({ source_name, SourceData(std::move(source_path)) });
            } else {
                it->second = SourceData(std::move(source_path));
            }
        }

        /** Get a source by name */
        SourceData *get_source(const std::string& source_name) {
            auto it = m_sources.find(source_name);
            return it == m_sources.end() ? nullptr : &it->second;
        }

        /** Count the number of message accrued */
        [[nodiscard]] size_t count_message() const {
            return m_messages.size();
        }

        /** Go through each message, calling the given function on it **/
        void for_each_message(const std::function<void(Message&)>& func) const {
            for (auto message : m_messages) {
                func(message);
            }
        }

        /** Go through each message, calling the given function on it. Only include messages which meet the minimum level. **/
        void for_each_message(const std::function<void(Message&)>& func, Message::Level min_level) const {
            for (auto message : m_messages) {
                if (message.get_level() >= min_level) {
                    func(message);
                }
            }
        }

        /** Add a message to the message queue */
        void add_message(const Message& message) {
            m_messages.push_back(message);
        }

        /** Add a message to the message queue */
        void add_message(Message::Level level, std::string msg, int idx, int len);

        /** Add a message to the message queue for the given source */
        void add_message(const std::string &source_name, Message::Level level, std::string msg, int idx, int len);

        /** Return whether or not there is a message of the given type in the message queue */
        bool has_message_of(Message::Level level) {
            return std::any_of(m_messages.begin(), m_messages.end(), [&level](Message &message) {
                return message.get_level() == level;
            });
        }

        void reset_row() {
            m_row = 0;
        }

        void clear_messages() {
            m_messages.clear();
        }

        [[nodiscard]] int get_row() const {
            return m_row;
        }

        int inc_row() {
            return ++m_row;
        }

        /** Get the output stream */
        [[nodiscard]] std::stringstream *out() {
            return &m_out_stream;
        }

        /** Get parser options */
        [[nodiscard]] Options *options() { return &m_options; }

        /** Build into the given source */
        void build();
    };
}
