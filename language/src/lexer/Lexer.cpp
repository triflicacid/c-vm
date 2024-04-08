#include <iostream>
#include "Lexer.hpp"

namespace language::lexer {
    void Lexer::add_token(Token::Type type) {
        add_token(type, tokens.find(type)->second);
    }

    void Lexer::add_token(Token::Type type, const std::string &image) {
        Location location(m_line, m_col);
        Token token(location, type, image);

        m_tokens.push_back(token);
    }

    bool Lexer::consume_newline() {
        if (exists() && peek() == '\r' && peek(1) == '\n') {
            move(2);
            newline();
        } else if (peek() == '\r' || peek() == '\n') {
            move(1);
            newline();
        } else {
            return false;
        }

        return true;
    }

    bool Lexer::consume_whitespace() {
        if (peek() == ' ' || peek() == '\t') {
            move(1);
        } else if (!consume_newline()) {
            return false;
        }

        return true;
    }

    bool Lexer::consume_comment() {
        if (exists() && peek() == '/') { // Both comment delimiters have length 2 and start with '/'
            if (peek(1) == '/') {
                // Single-line comment; continue until EOL or EOF
                while (exists() && !consume_newline()) {
                    move(1);
                }

                return true;
            }

            if (peek(1) == '*') {
                // Multi-line comment; continue until `*/` or EOF
                while (exists()) {
                    if (exists(1) && peek() == '*' && peek(1) == '/') {
                        move(2);
                        break;
                    }

                    if (!consume_newline()) {
                        move(1);
                    }
                }

                return true;
            }
        }

        return false;
    }

    message::MessageWithSource *Lexer::generate_token_error() {
        std::stringstream stream;
        stream << "Unexpected token '" << peek() << "'.";

        auto err = new message::MessageWithSource(message::Level::Error, m_path, m_line, m_col, m_col, 1, get_line(m_line));
        err->set_message(stream);
        return err;
    }

    void language::lexer::Lexer::lex(message::List &messages) {
        reset();
        int line_start_at = m_pos;

        while (exists()) {
            if (consume_newline()) {
                if (m_pos != line_start_at && !expect_after(m_tokens.back().type())) {
                    add_token(Token::Type::EOL, "");
                    line_start_at = m_pos;
                }

                continue;
            }

            if (consume_whitespace()) {
                continue;
            }

            // Do we have a comment?
            if (consume_comment()) {
                continue;
            }

            // Provided constant tokens?
            bool found_match = false;

            for (const auto &pair : tokens) {
                if (matches(pair.second)) {
                    add_token(pair.first);
                    move((int) pair.second.length());
                    found_match = true;
                    break;
                }
            }

            if (found_match)
                continue;

            // Integer?
            if (std::isdigit(peek())) {
                int n = 1;
                char c;

                // Extract [0-9]*
                while (exists(n) && (c = peek(n)) && std::isdigit(c)) {
                    n++;
                }

                add_token(Token::Type::INTEGER, extract(n));
                move(n);

                continue;
            }

            // Identifier?
            if (std::isalpha(peek())) {
               int n = 1;
               char c;

               // Extract [A-Za-z_]*
               while (exists(n) && (c = peek(n)) && (c == '_' || std::isalnum(c))) {
                   n++;
               }

               add_token(std::isupper(peek()) ? Token::Type::DATA_NAME : Token::Type::SYMBOL_NAME, extract(n));
               move(n);

               continue;
            }

            // Report error
            messages.add(generate_token_error());
            return;
        }
    }

    std::string Lexer::to_xml() {
        if (m_tokens.empty()) {
            return "";
        }

        std::stringstream stream;
        stream << "<Lines>" << std::endl << "  <Line>" << std::endl;
        int line_length = 0;

        for (auto token : m_tokens) {
            if (token.type() == Token::Type::EOL) {
                if (line_length > 0)
                    stream << "  </Line>" << std::endl << "  <Line>" << std::endl;

                line_length = 0;
            } else {
                stream << "    <Token line=\"" << token.location().line() << "\" col=\"" << token.location().column()
                       << "\" type=\""
                       << token.type() << "\">" << token.image() << "</Token>" << std::endl;
                line_length++;
            }
        }

        stream << "  </Line>" << std::endl << "</Lines>" << std::endl;

        return stream.str();
    }

    std::string Lexer::get_line(int n) {
        std::stringstream stream(m_string);
        std::string line;

        for (int i = 0; std::getline(stream, line); i++) {
            if (i == n) {
                return line;
            }
        }

        return line;
    }
}
