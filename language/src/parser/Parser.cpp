#include <iostream>
#include "Parser.hpp"

#include "messages/message.hpp"
#include "Symbol.hpp"
#include "VariableSymbol.hpp"
#include "types/NumericType.hpp"

namespace language::parser {
    void Parser::save() {
        m_pos_stack.push(m_pos);
    }

    void Parser::restore() {
        if (m_pos_stack.empty()) {
            m_pos = 0;
        } else {
            m_pos = m_pos_stack.top();
            m_pos_stack.pop();
        }
    }

    void Parser::discard() {
        m_pos_stack.pop();
    }

    bool Parser::expect(lexer::Token::Type type, message::List *messages) {
        if (exists() && peek()->type() == type) {
            move();
            return true;
        }

        if (messages != nullptr) {
            messages->add(generate_syntax_error({type}));
        }

        return false;
    }

    bool Parser::expect(const std::vector<lexer::Token::Type> &types, message::List *messages) {
        if (exists() && std::find(types.begin(), types.end(), peek()->type()) != types.end()) {
            move();
            return true;
        }

        if (messages != nullptr) {
            messages->add(generate_syntax_error(types));
        }

        return false;
    }

    bool Parser::expect(const std::function<bool(lexer::Token::Type)>& test_fn) {
        if (exists() && test_fn(peek()->type())) {
            move();
            return true;
        }

        return false;
    }

    message::MessageWithSource *Parser::generate_message(message::Level level, const std::string& message) {
        // Token we have an issue with
        const auto& token = peek();
        Location location = token->location();

        auto msg = new message::MessageWithSource(level, m_source->path(), location.line(),
                                                  location.column(), location.column(), (int) token->size(),
                                                  m_source->get_line(location.line()));
        msg->set_message(message);
        return msg;
    }

    message::MessageWithSource *Parser::generate_syntax_error(const std::vector<lexer::Token::Type>& expected) {
        // Construct message
        std::stringstream message;
        message << "Syntax Error: encountered " << lexer::Token::repr(peek()->type()) << ".";

        if (!expected.empty()) {
            message << " Expected ";

            if (expected.size() > 1) message << "one of ";

            for (int i = 0; i < expected.size(); i++) {
                message << lexer::Token::repr(expected[i]);

                if (i < expected.size() - 1) message << ", ";
            }

            message << '.';
        }

        return generate_error(message.str());
    }

    bool Parser::consume_kw_decl(message::List &messages) {
        if (expect(lexer::Token::KW_DECL)) {
            do {
                int identifier_pos = m_pos;

               // <IDENTIFIER>
               if (!expect(lexer::Token::IDENTIFIER, &messages)) {
                   return false;
               }

               auto token_identifier = peek(-1);

               // Check if name exists
               if (m_scope->exists(token_identifier->image())) {
                   const Symbol *old_symbol = m_scope->get(token_identifier->image())->first;

                   // If shadowing...
                   if (!options.allow_shadowing) {
                       move(-1);
                       messages.add(generate_error("Decl: identifier is already bound"));

                       set(old_symbol->position());
                       messages.add(generate_message(message::Level::Note, "Identifier bound here"));

                       return false;
                   }

                   // Check if not assignable
                   if (!old_symbol->can_be_assigned()) {
                       move(-1);
                       messages.add(generate_error("Decl: identifier cannot be assigned"));

                       set(old_symbol->position());
                       messages.add(generate_message(message::Level::Note, "Identifier bound here"));

                       return false;
                   }
               }

                // ":"
                if (!expect(lexer::Token::COLON, &messages)) {
                    return false;
                }

                // <TYPE>
                if (!expect(lexer::valid_types, &messages)) {
                    return false;
                }

                auto token_type = peek(-1);
                const types::Type *type;

                // If the type is a DATA_IDENTIFIER, check if it exists
                if (token_type->type() == lexer::Token::DATA_IDENTIFIER) {
                    if (m_scope->data_exists(token_type->image())) {
                        type = m_scope->data_get(token_type->image());
                    } else {
                        move(-1);
                        messages.add(generate_error("Decl: reference to undefined type"));
                        return false;
                    }
                } else {
                    type = new types::NumericType(token_type, m_pos - 1);
                }

                // Add to current scope, deleting old Symbol if needed
                auto symbol = new VariableSymbol(identifier_pos, token_identifier->image(), type);
                delete m_scope->create(symbol);

                // "," or <EOL>
                if (!expect({ lexer::Token::COMMA, lexer::Token::EOL }, &messages)) {
                    return false;
                }

                // If <EOL>, break, else scan for more
                if (peek(-1)->type() == lexer::Token::EOL) {
                    return true;
                }
            } while (true);
        } else {
            return false;
        }
    }

    bool Parser::consume_kw_data(message::List &messages) {
        if (expect(lexer::Token::KW_DATA)) {
            // <DATA IDENTIFIER>
            int data_pos = m_pos;

            if (!expect(lexer::Token::DATA_IDENTIFIER, &messages)) {
                return false;
            }

            auto token_data_identifier = peek(-1);

            // Check if already exists
            if (m_scope->data_exists(token_data_identifier->image())) {
                const types::UserType *old_type = m_scope->data_get(token_data_identifier->image());

                move(-1);
                messages.add(generate_error("Data: identifier is already bound"));

                set(old_type->position());
                messages.add(generate_message(message::Level::Note, "Identifier bound here"));

                return false;
            }

            // "{"
            if (!expect(lexer::Token::LBRACE, &messages)) {
                return false;
            }

            // Start constructing type
            auto user_type = new types::UserType(token_data_identifier, data_pos);

            // Parse contents
            while (true) {
                // <IDENTIFIER>
                int identifier_pos = m_pos;

                if (!expect(lexer::Token::IDENTIFIER, &messages)) {
                    return false;
                }

                auto token_identifier = peek(-1);

                // Check: does member already exist
                if (user_type->exists(token_identifier->image())) {
                    move(-1);
                    messages.add(generate_error("Data: duplicate member"));

                    set(user_type->get(token_identifier->image())->first->position());
                    messages.add(generate_message(message::Level::Note, "Member declared here"));

                    set(data_pos);
                    messages.add(generate_message(message::Level::Note, "Data declared here"));

                    return false;
                }

                // ":"
                if (!expect(lexer::Token::COLON, &messages)) {
                    return false;
                }

                // <TYPE>
                if (!expect(lexer::valid_types, &messages)) {
                    return false;
                }

                auto token_type = peek(-1);
                const types::Type *type;

                // If the type is a DATA_IDENTIFIER, check if it exists
                if (token_type->type() == lexer::Token::DATA_IDENTIFIER) {
                    if (m_scope->data_exists(token_type->image())) {
                        type = m_scope->data_get(token_type->image());
                    } else {
                        move(-1);
                        messages.add(generate_error("Reference to undefined type"));
                        return false;
                    }
                } else {
                    type = new types::NumericType(token_type, m_pos - 1);
                }

                // Add to current scope, deleting old Symbol if needed
                auto symbol = new VariableSymbol(identifier_pos, token_identifier->image(), type);
                user_type->add(symbol);

                // <EOL> or "," or "}"
                if (!expect({ lexer::Token::EOL, lexer::Token::COMMA, lexer::Token::RBRACE }, &messages)) {
                    return false;
                }

                // If '}', exit
                if (peek(-1)->type() == lexer::Token::RBRACE || (exists() && peek()->type() == lexer::Token::RBRACE)) {
                    move();
                    break;
                }
            }

            // Add to scope
            m_scope->data_create(user_type);

            return true;
        } else {
            return false;
        }
    }

    void Parser::parse(message::List &messages) {
        reset();

        // Check that a scope has been attached
        if (m_scope == nullptr) {
            return;
        }

        message::List sub_messages;
        std::vector<std::function<bool()>> parsers = {
            [this, &sub_messages] { return this->consume_kw_decl(sub_messages); },
            [this, &sub_messages] { return this->consume_kw_data(sub_messages); },
        };


        while (exists()) {
            // Skip <EOL?
            if (peek()->type() == lexer::Token::EOL) {
                move();
                continue;
            }

            // Iterate over parsers
            bool found_match = false;

            for (const auto& parser : parsers) {
                save();

                if (parser()) {
                    found_match = true;
                    discard();
                    break;
                } else {
                    if (sub_messages.has_message_of(message::Level::Error)) {
                        messages.append(sub_messages);
                        return;
                    }

                    sub_messages.clear();
                    restore();
                }
            }

            if (found_match) {
                continue;
            }

            messages.add(generate_syntax_error({lexer::Token::KW_DECL, lexer::Token::KW_DATA}));
            return;
        }
    }
}
