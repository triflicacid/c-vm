#pragma once

#include "Token.hpp"
#include "util/messages/list.hpp"
#include "messages/MessageWithSource.hpp"

#include <utility>
#include <vector>

namespace language::lexer {
    class Lexer {
    private:
        int m_line;
        int m_col;
        int m_pos;
        std::string m_string;
        std::filesystem::path m_path;
        std::vector<Token> m_tokens;

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

        /** Check if character exists at (pos+n). Default n=1. */
        bool exists(int n = 1) {
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

        /** Get the requested line (default to the last line) */
        std::string get_line(int n);

        /** Construct parse error. */
        message::MessageWithSource *generate_token_error();
    public:
        explicit Lexer(std::string string) : m_line(0), m_col(0), m_pos(0), m_path("<file>"), m_string(std::move(string)) {};

        /** Reset internal states. */
        void reset() {
            m_line = m_col = m_pos = 0;
            m_tokens.clear();
        }

        std::filesystem::path get_path() { return m_path; }

        void set_path(std::filesystem::path path) { m_path = std::move(path); }

        void lex(message::List &messages);

        /** Get token XML document. */
        std::string to_xml();
    };
}
