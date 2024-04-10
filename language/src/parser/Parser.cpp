#include <iostream>
#include "Parser.hpp"

#include "messages/message.hpp"
#include "Symbol.hpp"
#include "Symbol.hpp"
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

    bool Parser::check_can_create_identifier(const lexer::Token &identifier, int pos, message::List &messages) {
        auto local = m_scopes.get_local();
        bool exists = false;
        int other_pos;

        if (identifier.type() == lexer::Token::IDENTIFIER) {
            if (local->var_exists(identifier.image())) {
                exists = true;
                other_pos = local->var_get(identifier.image())->first->position();
            } else if (local->func_exists(identifier.image())) {
                exists = true;
                other_pos = local->func_get(identifier.image())->back()->position();
            }
        } else if (identifier.type() == lexer::Token::DATA_IDENTIFIER) {
            if (local->data_exists(identifier.image())) {
                exists = true;
                other_pos = local->data_get(identifier.image())->position();
            }
        }

        if (exists) {
            set(pos);
            messages.add(generate_error("Identifier is already bound"));

            set(other_pos);
            messages.add(generate_message(message::Level::Note, "Identifier bound here"));

            return false;
        }

        return true;
    }

    bool Parser::check_can_shadow_identifier(const lexer::Token &identifier, int pos, message::List &messages) {
        auto local = m_scopes.get_local();
        bool can_shadow = true;
        int other_pos;

        if (local->var_exists(identifier.image())) {
            if (!options.allow_shadowing) {
                can_shadow = false;
                other_pos = local->var_get(identifier.image())->first->position();
            }
        } else if (local->func_exists(identifier.image())) {
            can_shadow = false;
            other_pos = local->func_get(identifier.image())->back()->position();
        }

        if (can_shadow) {
            return true;
        } else {
            set(pos);
            messages.add(generate_error("Cannot shadow identifier"));

            set(other_pos);
            messages.add(generate_message(message::Level::Note, "Identifier bound here"));

            return false;
        }
    }

    bool Parser::check_can_create_overload(const std::string &name, const types::FunctionType *overload,
                                           message::List &messages) {
        if (m_scopes.get_local()->func_exists(name, overload)) {
            auto *old = m_scopes.get_local()->func_get(name, overload);

            set(overload->position());
            messages.add(generate_error("Signature already exists: " + name + overload->repr()));

            set(old->position());
            messages.add(generate_message(message::Level::Note, "Signature previously declared here."));

            return false;
        } else {
            return true;
        }
    }

    message::MessageWithSource *Parser::generate_message(message::Level level, const std::string& message) {
        // Token we have an issue with
        const auto& token = peek();
        Location location = token->location();

        auto msg = new message::MessageWithSource(level, m_prog->source()->path(), location.line(),
                                                  location.column(), location.column(), (int) token->size(),
                                                  m_prog->source()->get_line(location.line()));
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

    bool Parser::consume_type(message::List& messages, const types::Type **type) {
        if (!expect(lexer::valid_types, &messages)) {
            return false;
        }

        auto token_type = peek(-1);

        // If the type is a DATA_IDENTIFIER, check if it exists
        if (token_type->type() == lexer::Token::DATA_IDENTIFIER) {
            if (m_scopes.data_exists(token_type->image())) {
                *type = m_scopes.data_get(token_type->image());
            } else {
                move(-1);
                messages.add(generate_error("Decl: reference to undefined type"));
                return false;
            }
        } else {
            *type = new types::NumericType(token_type, m_pos - 1);
        }

        return true;
    }

    bool Parser::consume_kw_decl_func(message::List &messages) {
        if (expect(lexer::Token::KW_DECL) && expect(lexer::Token::KW_FUNC)) {
            // <IDENTIFIER>
            int func_name_pos = m_pos;

            if (!expect(lexer::Token::IDENTIFIER, &messages)) {
                return false;
            }

            auto token_func_name = peek(-1);

            // Collect parameters
            std::vector<const types::Type *> param_types;

            if (!expect(lexer::Token::UNIT) && expect(lexer::Token::LPARENS)) {
                const types::Type *param_type;

                while (true) {
                    if (!consume_type(messages, &param_type)) {
                        return false;
                    }

                    // Add to parameter list
                    param_types.push_back(param_type);

                    // Expect "," or ")"
                    if (!expect({lexer::Token::COMMA, lexer::Token::RPARENS}, &messages)) {
                        return false;
                    }

                    // If ")", exit
                    if (peek(-1)->type() == lexer::Token::RPARENS) {
                        break;
                    }
                }
            }

            // Parse return type
            const types::Type *return_type = nullptr;

            if (expect(lexer::Token::ARROW)) {
                if (!expect(lexer::Token::UNIT) && !consume_type(messages, &return_type)) {
                    return false;
                }
            }

            // <EOL>
            if (!expect(lexer::Token::EOL, &messages)) {
                if (peek()->type() == lexer::Token::LBRACE) {
                    set(func_name_pos - 2);
                    messages.add(generate_message(message::Level::Note, "Did you mean to define a function? Remove \"decl\"."));
                }

                return false;
            }

            // Create function type
            auto *type = new types::FunctionType(token_func_name, func_name_pos, param_types, return_type);

            // Check if the overload already exists
            if (!check_can_create_overload(token_func_name->image(), type, messages)) {
                delete type;
                return false;
            }

            // Create function & register with global table
            m_scopes.func_create(token_func_name->image(), type);

            auto *func = new statement::Function(type, m_prog->new_function_id());
            m_prog->register_function(func);

            return true;
        } else {
            return false;
        }
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

               // Perform checks
               if (!check_can_create_identifier(*token_identifier, identifier_pos, messages) || !check_can_shadow_identifier(*token_identifier, identifier_pos, messages)) {
                   return false;
               }

                // ":"
                if (!expect(lexer::Token::COLON, &messages)) {
                    return false;
                }

                // Type
                const types::Type *type;
                if (!consume_type(messages, &type)) {
                    return false;
                }

                // Add to current scope, deleting old Symbol if needed
                auto symbol = new Symbol(identifier_pos, token_identifier->image(), type);
                delete m_scopes.var_create(symbol);

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

            // Perform checks
            if (!check_can_create_identifier(*token_data_identifier, data_pos, messages)) {
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

                // Type
                const types::Type *type;
                if (!consume_type(messages, &type)) {
                    return false;
                }

                // Add to current scope, deleting old Symbol if needed
                auto symbol = new Symbol(identifier_pos, token_identifier->image(), type);
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
            m_scopes.data_create(user_type);

            return true;
        } else {
            return false;
        }
    }

    bool Parser::consume_kw_func(message::List &messages) {
        if (expect(lexer::Token::KW_FUNC)) {
            // <IDENTIFIER>
            int func_name_pos = m_pos;

            if (!expect(lexer::Token::IDENTIFIER, &messages)) {
                return false;
            }

            auto token_func_name = peek(-1);

            // Collect parameters
            std::vector<const types::Type *> param_types;
            std::vector<std::string> param_names;
            std::map<std::string, int> param_name_positions;

            if (!expect(lexer::Token::UNIT) && expect(lexer::Token::LPARENS)) {
                const types::Type *param_type;

                while (true) {
                    // <IDENTIFIER>
                    int param_pos = m_pos;

                    if (!expect(lexer::Token::IDENTIFIER, &messages)) {
                        return false;
                    }

                    std::string param_name = peek(-1)->image();

                    // Does parameter already exist?
                    auto old_param_pos = param_name_positions.find(param_name);

                    if (old_param_pos != param_name_positions.end()) {
                        move(-1);
                        messages.add(generate_error("Duplicate parameter " + param_name));

                        set(old_param_pos->second);
                        messages.add(generate_message(message::Level::Note, "Parameter first appeared here"));

                        set(func_name_pos);
                        messages.add(generate_message(message::Level::Note, "In function " + token_func_name->image()));

                        return false;
                    }

                    // Record parameter
                    param_names.push_back(param_name);
                    param_name_positions.insert({ param_name, param_pos });

                    // ":"
                    if (!expect(lexer::Token::COLON, &messages)) {
                        return false;
                    }

                    // Type
                    if (!consume_type(messages, &param_type)) {
                        return false;
                    }

                    // Add to parameter list
                    param_types.push_back(param_type);

                    // Expect "," or ")"
                    if (!expect({lexer::Token::COMMA, lexer::Token::RPARENS}, &messages)) {
                        return false;
                    }

                    // If ")", exit
                    if (peek(-1)->type() == lexer::Token::RPARENS) {
                        break;
                    }
                }
            }

            // Parse return type
            const types::Type *return_type = nullptr;

            if (expect(lexer::Token::ARROW)) {
                if (!expect(lexer::Token::UNIT) && !consume_type(messages, &return_type)) {
                    return false;
                }
            }

            // "{"
            if (!expect(lexer::Token::LBRACE, &messages)) {
                // Helpful message if declaration attempted
                if (peek()->type() == lexer::Token::EOL) {
                    set(func_name_pos - 1);
                    messages.add(generate_message(message::Level::Note, "Did you mean to declare a function? Prefix with \"decl\"."));
                }

                return false;
            }

            // Create function type
            auto *type = new types::FunctionType(token_func_name, func_name_pos, param_types, return_type);

            // Check if function was declared previously
            auto *old_type = m_scopes.get_local()->func_get(token_func_name->image(), type);

            statement::Function *func;

            if (old_type == nullptr) {
                // Strict: declaration must be present before definition
                if (options.must_declare_functions) {
                    set(func_name_pos);
                    messages.add(generate_error("Attempted to define function before declaration"));
                    messages.add(generate_message(message::Level::Note,
                                                  "Signature: " + token_func_name->image() + type->repr()));

                    delete type;
                    return false;
                }

                // Create declaration quickly
                m_scopes.func_create(token_func_name->image(), type);
                func = new statement::Function(type, m_prog->new_function_id());
                m_prog->register_function(func);
            } else {
                // Delete old type, not needed anymore
                delete type;

                func = m_prog->get_function(old_type->id());

                // Check if function is already defined
                if (func->is_complete()) {
                    set(func_name_pos);
                    messages.add(generate_error("Cannot re-define function " + token_func_name->image() + type->repr()));

                    set(old_type->position());
                    messages.add(generate_message(message::Level::Note, "Function defined here"));

                    delete type;
                    return false;
                }
            }

            // TODO parse body
            // For now, empty body
            if (!expect(lexer::Token::RBRACE, &messages)) {
                return false;
            }

            func->complete_definition(param_names, {});

            return true;
        } else {
            return false;
        }
    }

    void Parser::parse(message::List &messages) {
        reset();

        // Ensure a scope exists -- global scope
        if (m_scopes.size() == 0) {
            m_scopes.push();
        }

        message::List sub_messages;
        std::vector<std::function<bool()>> parsers = {
            [this, &sub_messages] { return this->consume_kw_decl_func(sub_messages); },
            [this, &sub_messages] { return this->consume_kw_decl(sub_messages); },
            [this, &sub_messages] { return this->consume_kw_data(sub_messages); },
            [this, &sub_messages] { return this->consume_kw_func(sub_messages); },
        };


        while (exists()) {
            // Skip <EOL> ?
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

            messages.add(generate_syntax_error({lexer::Token::KW_DECL, lexer::Token::KW_DATA, lexer::Token::KW_FUNC}));
            return;
        }
    }
}
