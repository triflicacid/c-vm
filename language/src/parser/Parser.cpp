#include <iostream>
#include "Parser.hpp"

#include "language/src/statement/Symbol.hpp"
#include "language/src/statement/Symbol.hpp"
#include "types/NumericType.hpp"
#include "statement/Number.hpp"
#include "statement/UnaryOperator.hpp"
#include "statement/BinaryOperator.hpp"
#include "statement/ReturnStatement.hpp"
#include "LanguageOptions.hpp"

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
            if (!options::allow_shadowing && local->var_exists(identifier.image())) {
                exists = true;
                other_pos = local->var_get(identifier.image())->position();
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
            if (!options::allow_shadowing) {
                can_shadow = false;
                other_pos = local->var_get(identifier.image())->position();
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

    bool Parser::check_type_match(const types::Type *a, const types::Type *b, int pos, message::List *messages) {
        bool match = true;

        // b is empty, a is not
        if (b == nullptr) {
            if (a != nullptr) {
                match = false;

                if (messages) {
                    set(pos);
                    messages->add(generate_error("Type Mismatch: cannot match type " + a->repr() + " with ()"));
                }
            }
        }

        // a is empty, b is not
        else if (a == nullptr) {
            match = false;

            if (messages) {
                mark_end();
                set(pos);
                messages->add(generate_error("Type Mismatch: cannot match () with type " + b->repr()));
            }
        }

        // If either are not numeric or cannot implicitly cast between them
        else if (a->category() != types::Type::Category::Numeric || b->category() != types::Type::Category::Numeric
                || !types::NumericType::can_implicitly_cast_to((types::NumericType *) a, (types::NumericType *) b)) {
            match = false;

            if (messages) {
                mark_end();
                set(pos);
                messages->add(generate_error("Type Mismatch: cannot match type " + b->repr() + " with type " + a->repr()));
            }
        }

        if (!match) {
            set_end(-1);
        }

        return match;
    }

    bool Parser::check_symbol_unused(const parser::SymbolDeclaration *symbol, message::List& messages) {
        auto level = options::unused_symbol_level < 0 ? message::Level::Note : message::level_from_int(options::unused_symbol_level);
        bool unused = false;
        save(); // Ensure position isn't corrupted

        if (!symbol->was_used_since_assignment) {
            if (!symbol->was_assigned) {
                set(symbol->position());
                messages.add(generate_message(level, "Unused symbol, consider removing"));
                unused = true;
            } else {
                set(symbol->last_assigned_pos);
                messages.add(generate_message(level, "Assigned value never used, consider removing"));
                unused = true;
            }
        }

        restore();
        return unused;
    }

    message::MessageWithSource *Parser::generate_message(message::Level level, const std::string& message) {
        // Token we have an issue with
        const auto& token = peek();
        Location location = token->location();

        // Record end column.
        size_t end_col = location.column() + token->size();

        if (m_end_pos > m_pos) {
          for (int i = m_pos + 1; i < m_end_pos; i++) {
              if (m_tokens[i].location().line() != location.line()) {
                  break;
              }

              end_col = m_tokens[i].location().column() + m_tokens[i].size();
          }
        }

        auto msg = new message::MessageWithSource(level, m_prog->source()->path(), location.line(),
                                                  location.column(), location.column(), (int) (end_col - location.column()),
                                                  m_prog->source()->get_line(location.line()));
        msg->set_message(message);
        return msg;
    }

    message::MessageWithSource *Parser::generate_syntax_error_multi(const std::vector<const std::vector<lexer::Token::Type> *>& expected) {
        std::stringstream message;
        message << "Syntax Error: encountered " << lexer::Token::repr(peek()->type()) << ".";

        size_t expected_count = 0;
        for (auto &vec : expected) {
            expected_count += vec->size();
        }

        if (expected_count > 0) {
            message << " Expected ";

            if (expected_count > 1) message << "one of ";

            for (auto &vec : expected) {
                for (int i = 0; i < vec->size(); i++) {
                    message << lexer::Token::repr((*vec)[i]);

                    if (i < expected_count - 1) message << ", ";
                }
            }

            message << '.';
        }

        return generate_error(message.str());
    }

    message::MessageWithSource *Parser::generate_syntax_error(const std::vector<lexer::Token::Type>& expected) {
        return generate_syntax_error_multi({&expected});
    }

    message::MessageWithSource *Parser::generate_custom_syntax_error(const std::string& expected) {
        std::stringstream message;
        message << "Syntax Error: encountered " << lexer::Token::repr(peek()->type()) << ". Expected " << expected << ".";
        return generate_error(message.str());
    }

    bool Parser::consume_type(message::List& messages, const types::Type **type) {
        if (expect(lexer::num_types)) {
            *type = &types::num_types.find(peek(-1)->type())->second;
        } else if (expect(lexer::Token::DATA_IDENTIFIER)) {
            if (m_scopes.data_exists(peek(-1)->image())) {
                *type = m_scopes.data_get(peek(-1)->image());
            } else {
                move(-1);
                messages.add(generate_error("Decl: reference to undefined type"));
                return false;
            }
        } else {
            std::vector<lexer::Token::Type> data_identifier = {lexer::Token::DATA_IDENTIFIER};
            messages.add(generate_syntax_error_multi({&lexer::num_types, &data_identifier}));
            return false;
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

            // Parse function signature type
            auto *type = parse_function_type(messages, false, func_name_pos);

            if (type == nullptr) {
                return false;
            }

            // <EOL>
            if (!expect(lexer::Token::EOL, &messages)) {
                if (peek()->type() == lexer::Token::LBRACE) {
                    set(func_name_pos - 2);
                    messages.add(generate_message(message::Level::Note, "Did you mean to define a function? Remove \"decl\"."));
                }

                return false;
            }

            // Check if the overload already exists
            if (!check_can_create_overload(token_func_name->image(), type, messages)) {
                delete type;
                return false;
            }

            // Create function & register with global table
            m_scopes.get_local()->func_create(token_func_name->image(), type);

            auto *func = new statement::Function(token_func_name->image(), type, m_prog->new_function_id());
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

               // Shadow unused symbol warning
               if (options::unused_symbol_level > -1) {
                   auto exists = m_scopes.get_local()->var_get(token_identifier->image());

                   if (exists != nullptr && !exists->was_assigned) {
                       auto level = message::level_from_int(options::unused_symbol_level);

                       save();
                       set(exists->position());
                       messages.add(generate_message(level, "Unused symbol, consider removing"));
                       restore();

                       messages.add(generate_message(message::Level::Note, "Being shadowed here"));

                       if (level == message::Level::Error) {
                           return false;
                       }
                   }
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
                auto symbol = new parser::SymbolDeclaration(identifier_pos, token_identifier->image(), type);
                delete m_scopes.get_local()->var_create(symbol);

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

                    set(user_type->get(token_identifier->image())->position());
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
                auto symbol = new parser::SymbolDeclaration(identifier_pos, token_identifier->image(), type);
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
            m_scopes.get_local()->data_create(user_type);

            return true;
        } else {
            return false;
        }
    }

    bool Parser::consume_kw_return(message::List &messages, statement::StatementBlock& block, bool check_return_type) {
        if (expect(lexer::Token::KW_RETURN)) {
            int pos = m_pos - 1;
            statement::ReturnStatement *stmt;
            bool expect_expr;

            // "()" <EOL> or <EOL> for no value, else <expr>
            if (expect(lexer::Token::UNIT)) {
                if (expect(lexer::Token::EOL, &messages)) {
                    expect_expr = false;
                } else {
                    return false;
                }
            } else {
                expect_expr = !expect(lexer::Token::EOL);
            }

            if (expect_expr) {
                auto *expr = parse_expression(messages);

                if (messages.has_message_of(message::Level::Error)) {
                    return false;
                }

                stmt = new statement::ReturnStatement(pos, expr);
            } else {
                stmt = new statement::ReturnStatement(pos, nullptr);
            }

            // Check to see if return type matches with current function
            if (check_return_type) {
                auto *function = m_prog->get_function(m_scopes.get_local()->invoker_id());

                if (!check_type_match(stmt->get_type_of(), function->function_type()->return_type(), pos, &messages)) {
                    set_end(-1);
                    set(function->position());
                    messages.add(generate_message(message::Level::Note, "In function " + function->name() + function->function_type()->repr()));

                    delete stmt;
                    return false;
                }
            }

            block.add(stmt);
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
            auto *type = new types::FunctionType(func_name_pos, param_types, return_type);

            // Check if function was declared previously
            auto *old_type = m_scopes.get_local()->func_get(token_func_name->image(), type);

            statement::Function *func;

            if (old_type == nullptr) {
                // Strict: declaration must be present before definition
                if (options::must_declare_functions) {
                    set(func_name_pos);
                    messages.add(generate_error("Attempted to define function before declaration"));
                    messages.add(generate_message(message::Level::Note,
                                                  "Signature: " + token_func_name->image() + type->repr()));

                    delete type;
                    return false;
                }

                // Create declaration quickly
                m_scopes.get_local()->func_create(token_func_name->image(), type);
                func = new statement::Function(token_func_name->image(), type, m_prog->new_function_id());
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

            // Create new scope
            m_scopes.push(func->id());

            // Parse body as code block
            auto *block = parse_code_block(messages);

            if (messages.has_message_of(message::Level::Error)) {
//                set(func_name_pos);
//                messages.add(generate_message(message::Level::Note, "In function " + token_func_name->image()));
                delete block;

                return false;
            }

            func->complete_definition(param_names, block);

            // TODO check for unused variables, functions completely
            if (options::unused_symbol_level > -1) {
                for (auto &symbol: m_scopes.get_local()->symbols()) {
                    if (check_symbol_unused(symbol, messages) && messages.has_message_of(message::Level::Error)) {
                        return false;
                    }
                }
            }

            // Remove old scope - all sorted
            m_scopes.pop();

            return true;
        } else {
            return false;
        }
    }

    bool Parser::consume_kw_entry(message::List &messages) {
        if (expect(lexer::Token::KW_ENTRY)) {
            // Has an entry already been specified?
            if (m_entry_pos > -1) {
                move(-1);
                messages.add(generate_error("Entry point has already been defined"));

                set(m_entry_pos);
                messages.add(generate_message(message::Level::Note, "Previously defined here"));

                return false;
            }

            // <IDENTIFIER>
            int pos = m_pos;

            if (!expect(lexer::Token::IDENTIFIER, &messages)) {
                return false;
            }

            m_entry_pos = pos - 1;
            m_entry_name = peek(-1)->image();

            // Provide signature?
            if (expect({lexer::Token::LPARENS, lexer::Token::UNIT})) {
                move(-1);

                auto *entry_type = parse_function_type(messages, true, pos);

                if (entry_type == nullptr) {
                    return false;
                }

                m_entry_type = entry_type;
            }

            // <EOL>? (include '(' and '()' for message purposes only)
            if (!expect({lexer::Token::EOL, lexer::Token::LPARENS, lexer::Token::UNIT}, &messages)) {
                return false;
            }

            return true;
        } else {
            return false;
        }
    }

    const statement::Expression *Parser::parse_expression(message::List& messages, int precedence) {
        // Collect LHS
        const statement::Expression *lhs = nullptr;

        if (!exists()) {
            messages.add(generate_custom_syntax_error("expression"));
            return lhs;
        }

        // Literal?
        if (expect(lexer::Token::INTEGER)) {
            lexer::Token::Type num_type;

            // Followed by a type?
            if (expect(lexer::Token::COLON)) {
                if (!expect(lexer::num_types, &messages)) {
                    delete lhs;
                    return nullptr;
                }

                num_type = peek(-1)->type();
            } else {
                std::stringstream stream(peek(-1)->image());
                unsigned long long value;
                stream >> value;

                num_type = types::NumericType::get_suitable_type(value);
            }

            lhs = new statement::Number(m_pos - 1, peek(-1), num_type);
        } else if (expect(lexer::Token::IDENTIFIER)) {
            auto *identifier = peek(-1);
            int pos = m_pos - 1;

            if (m_scopes.var_exists(identifier->image())) {
                auto *decl = m_scopes.var_get(identifier->image());
                decl->was_used_since_assignment = true;
                auto *symbol = new statement::Symbol(pos, identifier, decl);
                lhs = symbol;
            } else if (m_scopes.func_exists(identifier->image())) {
                // TODO
                set(pos);
                messages.add(generate_error("Function calls are not supported."));
                return nullptr;
            } else {
                set(pos);

                messages.add(generate_error("Unbound identifier"));
                return nullptr;
            }
        } else if (expect(lexer::Token::LPARENS)) {
            lhs = parse_expression(messages);

            if (messages.has_message_of(message::Level::Error)) {
                delete lhs;
                return nullptr;
            }

            if (!expect(lexer::Token::RPARENS, &messages)) {
                delete lhs;
                return nullptr;
            }
        } else {
            auto op_token = peek();
            int op_pos = m_pos;
            auto op = token_unary_operators.find(op_token->type());

            if (op == token_unary_operators.end()) {
                // Build vector of expected tokens
                std::vector<lexer::Token::Type> expected = {lexer::Token::IDENTIFIER, lexer::Token::INTEGER, lexer::Token::LPARENS};

                for (auto& pair : token_unary_operators) {
                    expected.push_back(pair.first);
                }

                messages.add(generate_syntax_error(expected));
                return nullptr;
            } else {
                move();
                lhs = parse_expression(messages, precedence);

                if (messages.has_message_of(message::Level::Error)) {
                    delete lhs;
                    return nullptr;
                }

                lhs = new statement::UnaryOperator(op_pos, op_token, op->second.type, lhs);
            }
        }

        const OperatorInfo *op_info;
        const statement::Expression *rhs;
        const lexer::Token *op_token;
        int op_pos;

        while (exists() && peek()->type() != lexer::Token::EOL && peek()->type() != lexer::Token::RPARENS) {
            // Expect infix operator
            op_token = peek();
            op_pos = m_pos;
            auto op = token_binary_operators.find(op_token->type());

            if (op == token_binary_operators.end()) {
                // Build vector of binary operator types to report in an error
                std::vector<lexer::Token::Type> expected;
                expected.reserve(token_binary_operators.size());

                for (auto &pair: token_binary_operators) {
                    expected.push_back(pair.first);
                }

                messages.add(generate_syntax_error(expected));
                delete lhs;

                return nullptr;
            }

            op_info = &op->second;

            // Exit if higher precedence
            if (precedence >= op_info->precedence) {
                break;
            }

            move();

            // Parse remainder of expression
            rhs = parse_expression(messages, op_info->precedence - (op_info->right_associative ? 1 : 0));

            if (messages.has_message_of(message::Level::Error)) {
                delete rhs;
                delete lhs;
                return nullptr;
            }

            // If we have a symbol, perform some checks
            if (lhs->type() == statement::Type::SYMBOL) {
                auto *symbol = m_scopes.get_local()->var_get(((statement::Symbol *) lhs)->name());

                if (op_info->type == statement::OperatorType::ASSIGNMENT) {
                    symbol->was_assigned = true;
                    symbol->was_used_since_assignment = false;
                    symbol->last_assigned_pos = op_pos;

                    // Check if types match
                    if (!check_type_match(rhs->get_type_of(), symbol->type(), op_pos, &messages)) {
                        set(symbol->position());
                        messages.add(generate_message(message::Level::Note, "Identifier declared here"));

                        delete lhs;
                        delete rhs;
                        return nullptr;
                    }
                } else {
                    symbol->was_used_since_assignment = true;
                }
            }

            // Wrap in operator
            lhs = new statement::BinaryOperator(op_pos, op_token, op_info->type, lhs, rhs);
        }

        return lhs;
    }

    types::FunctionType *Parser::parse_function_type(message::List &messages, bool arg_list_required, int given_pos) {
        int pos = m_pos;

        // Collect parameters
        std::vector<const types::Type *> param_types;

        if (expect(lexer::Token::UNIT));
        else if (expect(lexer::Token::LPARENS)) {
            const types::Type *param_type;

            while (true) {
                if (!consume_type(messages, &param_type)) {
                    return nullptr;
                }

                // Add to parameter list
                param_types.push_back(param_type);

                // Expect "," or ")"
                if (!expect({lexer::Token::COMMA, lexer::Token::RPARENS}, &messages)) {
                    return nullptr;
                }

                // If ")", exit
                if (peek(-1)->type() == lexer::Token::RPARENS) {
                    break;
                }
            }
        } else if (arg_list_required) {
            messages.add(generate_syntax_error({lexer::Token::LPARENS, lexer::Token::UNIT}));
            return nullptr;
        }

        // Parse return type
        const types::Type *return_type = nullptr;

        if (expect(lexer::Token::ARROW)) {
            if (!expect(lexer::Token::UNIT) && !consume_type(messages, &return_type)) {
                return nullptr;
            }
        }

        return new types::FunctionType(given_pos == -1 ? pos : given_pos, param_types, return_type);
    }

    statement::StatementBlock *Parser::parse_code_block(message::List &messages) {
        auto *block = new statement::StatementBlock(m_pos);

        std::vector<std::function<bool()>> parsers = {
                [this, &messages, &block] { return this->consume_kw_return(messages, *block, true); },
                [this, &messages] { return this->consume_kw_decl(messages); },
        };

        while (true) {
            // Skip <EOL>
            if (expect(lexer::Token::EOL)) {
                continue;
            }

            // Terminate on "}"
            if (expect(lexer::Token::RBRACE)) {
                break;
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
                    if (messages.has_message_of(message::Level::Error)) {
                        delete block;
                        return nullptr;
                    }

                    restore();
                }
            }

            if (found_match) {
                continue;
            }

            // Last resort: expression
            auto *expr = parse_expression(messages);

            if (messages.has_message_of(message::Level::Error)) {
                break;
            }

            block->add(expr);
        }

        return block;
    }

    void Parser::parse(message::List &messages) {
        reset();

        // Ensure a scope exists -- global scope
        if (m_scopes.size() == 0) {
            m_scopes.push(-1);
        }

        std::vector<std::function<bool()>> parsers = {
            [this, &messages] { return this->consume_kw_decl_func(messages); },
            [this, &messages] { return this->consume_kw_data(messages); },
            [this, &messages] { return this->consume_kw_func(messages); },
            [this, &messages] { return this->consume_kw_entry(messages); },
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
                    discard();
                    found_match = true;
                    break;
                } else {
                    if (messages.has_message_of(message::Level::Error)) {
                        return;
                    }

                    restore();
                }
            }

            if (found_match) {
                continue;
            }

            messages.add(generate_syntax_error({lexer::Token::KW_DECL, lexer::Token::KW_DATA, lexer::Token::KW_FUNC, lexer::Token::KW_ENTRY}));
            return;
        }

        // Check entry point
        if (!check_entry_point_exists(messages)) {
            return;
        }
    }

    bool Parser::check_entry_point_exists(message::List &messages) {
        if (m_entry_pos < 0) return true; // Is not defined, so cannot check

        // If a name is provided, check it exists
        if (!m_scopes.get_local()->func_exists(m_entry_name)) {
            set(m_entry_pos + 1);
            messages.add(generate_error("Entry point " + m_entry_name + " cannot be found"));
            return false;
        }

        auto *overloads = m_scopes.get_local()->func_get(m_entry_name);

        // Either search for a valid option, or check is the specified one exists
        if (m_entry_type == nullptr) {
            for (auto& overload : *overloads) {
                if (types::FunctionType::valid_entry_point(overload)) {
                    m_entry_type = overload;
                    break;
                }
            }

            if (m_entry_type == nullptr) {
                set(m_entry_pos + 1);
                messages.add(generate_error("No suitable signature for entry point " + m_entry_name + " exists"));

                for (auto& overload : *overloads) {
                    set(overload->position());
                    messages.add(generate_message(message::Level::Note, "Invalid entry point signature"));
                }

                return false;
            }
        } else {
            bool found = false;

            for (auto& overload : *overloads) {
                if (overload->equal(*m_entry_type)) {
                    delete m_entry_type;
                    m_entry_type = overload;
                    found = true;
                    break;
                }
            }

            if (!found) {
                set(m_entry_pos + 1);
                set_end((int) m_tokens.size());
                messages.add(generate_error("Entry point " + m_entry_name + m_entry_type->repr() + " cannot be found"));
                set_end(-1);

                for (auto& overload : *overloads) {
                    set(overload->position());
                    messages.add(generate_message(message::Level::Note, "Invalid candidate"));
                }

                return false;
            }
        }

        // Check that entry point is defined
        if (!(m_entry_type->is_stored() && m_prog->get_function(m_entry_type->id())->is_complete())) {
            set(m_entry_pos + 1);
            messages.add(generate_error("Entry point " + m_entry_name + " must be bound"));

            set(m_entry_type->position());
            messages.add(generate_message(message::Level::Note, "Declared here"));

            return false;
        }

        return true;
    }

    std::map<lexer::Token::Type, OperatorInfo> token_binary_operators = {
            { lexer::Token::EQUALS, { 4, true, statement::OperatorType::ASSIGNMENT } },
            { lexer::Token::PLUS, { 14, false, statement::OperatorType::ADDITION } },
            { lexer::Token::DASH, { 14, false, statement::OperatorType::SUBTRACTION } },
            { lexer::Token::STAR, { 15, false, statement::OperatorType::MULTIPLICATION } },
    };

    std::map<lexer::Token::Type, OperatorInfo> token_unary_operators = {
            { lexer::Token::DASH, { 7, false, statement::OperatorType::NEGATION } },
    };
}
