#pragma once

#include "Token.hpp"
#include "util/messages/list.hpp"
#include "messages/MessageWithSource.hpp"

#include <utility>
#include <vector>
#include "../Source.hpp"

namespace language::lexer {
    class Lexer {
    private:
        int m_line;
        int m_col;
        int m_pos;
        Source *m_source;
        std::string m_string;

        /** Add token with the given type (must be in .tokens map) */
        void add_token(Token::Type type);

        /** Add token with the given type and image. */
        void add_token(Token::Type type, const std::string &image);

        /** Increment horizontal position. */
        void move(int x = 1) {
            m_col += x;
            m_pos += x;
        }

        /** Encounter a newline. */
        void newline() {
            m_col = 0;
            m_line++;
        }

        /** Check if character var_exists at (pos+n). Default n=0. */
        bool exists(int n = 0) {
            return m_pos + n < m_string.length();
        }

        /** Get character at (pos+n). Default n=0. */
        char peek(int n = 0) {
            return m_string[m_pos + n];
        }

        /** Extract `n` characters from current position. (extract(1) is same as peek()) */
        std::string extract(int n) {
            return m_string.substr(m_pos, n);
        }

        /** Check if the string has the given string at this position. */
        bool matches(const std::string& str) {
            if (m_string.length() - m_pos < str.length()) {
                return false;
            }

            return m_string.substr(m_pos, str.length()) == str;
        }

        /** Consume newline character. */
        bool consume_newline();

        /** Consume whitespace character, including newlines. */
        bool consume_whitespace();

        /** Consume a single- or multi-line comment */
        bool consume_comment();

        /** Construct parse error. */
        message::MessageWithSource *generate_token_error();

        /** Generate bracket mismatch error. */
        void generate_bracket_mismatch_error(lexer::Token& open_bracket, lexer::Token& closing_bracket, message::List& list);
    public:
        explicit Lexer(Source *source) : m_line(0), m_col(0), m_pos(0), m_source(source), m_string(source->contents()) {};

        /** Reset internal states. */
        void reset() {
            m_line = m_col = m_pos = 0;
            m_source->tokens.clear();
        }

        void lex(message::List &messages);

        void debug_print(std::ostream& stream, const std::string& prefix = "");
    };
}
