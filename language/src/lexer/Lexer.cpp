#include <iostream>
#include <stack>
#include "Lexer.hpp"

namespace language::lexer {
    void Lexer::add_token(Token::Type type) {
        add_token(type, tokens.find(type)->second);
    }

    void Lexer::add_token(Token::Type type, const std::string &image) {
        Location location(m_line, m_col);
        Token token(location, type, image);

        m_source->tokens.push_back(token);
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

        auto err = new message::MessageWithSource(message::Level::Error, m_source->path(), m_line, m_col, m_col, 1, m_source->get_line(m_line));
        err->set_message(stream);
        return err;
    }

    void Lexer::generate_bracket_mismatch_error(lexer::Token& open_bracket, lexer::Token& closing_bracket, message::List& list) {
        std::stringstream stream;
        stream << "Bracket mismatch: expected " << lexer::Token::repr((lexer::Token::Type) (open_bracket.type() + 1))
            << " to close " << lexer::Token::repr(open_bracket.type()) << ", got " << lexer::Token::repr(closing_bracket.type()) << ".";

        Location closing_loc = closing_bracket.location();
        Location opening_loc = open_bracket.location();

        message::Message *msg = new message::MessageWithSource(message::Level::Error, m_source->path(), closing_loc.line(),
                                                  closing_loc.column(), closing_loc.column(), 1,
                                                  m_source->get_line(closing_loc.line()));
        msg->set_message(stream);
        list.add(msg);

        msg = new message::MessageWithSource(message::Level::Note, m_source->path(), opening_loc.line(),
                                             opening_loc.column(), opening_loc.column(), 1,
                                             m_source->get_line(opening_loc.line()));
        msg->set_message("Bracket group opened here");
        list.add(msg);
    }

    void language::lexer::Lexer::lex(message::List &messages) {
        reset();
        int line_start_at = m_pos;
        std::stack<lexer::Token> bracket_stack;
        int open_bracket_paren_count = 0; // Count nest level of '(' and '['

        while (exists()) {
            if (consume_newline()) {
                if (m_pos != line_start_at) {
                    auto type = m_source->tokens.back().type();

                    if (!(open_bracket_paren_count > 0 || lexer::Token::is_operator(type) || lexer::Token::is_opening_bracket(type))) {
                        add_token(Token::Type::EOL, "");
                        line_start_at = m_pos;
                    }
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

            if (found_match) {
                auto& token = m_source->tokens.back();

                // Handle brackets
                if (lexer::Token::is_opening_bracket(token.type())) {
                    bracket_stack.push(token);

                    if (token.type() != lexer::Token::LBRACE) {
                        open_bracket_paren_count++;
                    }
                } else if (lexer::Token::is_closing_bracket(token.type())) {
                    // Check that this matches with the topmost bracket on the stack
                    if (lexer::Token::is_bracket_pair(bracket_stack.top().type(), token.type())) {
                        bracket_stack.pop();

                        if (token.type() != lexer::Token::RBRACE) {
                            open_bracket_paren_count--;
                        }
                    } else {
                        generate_bracket_mismatch_error(bracket_stack.top(), token, messages);
                        return;
                    }
                }

                continue;
            }

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

               add_token(std::isupper(peek()) ? Token::Type::DATA_IDENTIFIER : Token::Type::IDENTIFIER, extract(n));
               move(n);

               continue;
            }

            // Report error
            messages.add(generate_token_error());
            return;
        }

        // Bracket stack should be empty
        if (!bracket_stack.empty()) {
            Token eol({ m_line, m_col }, Token::EOL, "");
            generate_bracket_mismatch_error(bracket_stack.top(), eol, messages);
            return;
        }

        // Add final EOL
        if (!m_source->tokens.empty() && m_source->tokens.back().type() != Token::EOL) {
            add_token(Token::EOL, "");
        }
    }

    std::string Lexer::to_xml() {
        if (m_source->tokens.empty()) {
            return "";
        }

        std::stringstream stream;
        stream << "<Lines>" << std::endl << "  <Line>" << std::endl;
        int line_length = 0;

        for (const auto& token : m_source->tokens) {
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
}
