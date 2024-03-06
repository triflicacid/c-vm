#include <string>
#include <stack>
#include <cmath>
#include "Parser.h"
#include "tokens/NumericLiteral.h"
#include "tokens/SymbolDecl.h"
#include "util.h"
#include "tokens/BracketLiteral.h"
#include "tokens/SymbolReference.h"
#include "tokens/Print.h"
#include "types/NumericType.h"
#include "types/integer/Integer.h"
#include "types/Static.h"
#include "types/integer/Buffer.h"
#include <utility>
#include <sstream>
#include <iostream>

extern "C" {
#include "util/util.h"
}


namespace language::parser {
    void Parser::add_msg(Message::Level level, std::string msg, int idx, int len) {
        if (m_data == nullptr) {
            std::cerr << "m_data is NULL in add_msg" << std::endl;
            std::exit(1);
        }

        m_data->add_message(level, std::move(msg), idx, len);
    }

    int Parser::parse_numeric_type(const std::string &source, int idx, types::NumericType **type) {
        // Get length of substring which may make up the numeric token type
        size_t length = source.length() - idx;

        // Must be at least two characters
        if (length < 2) return idx;

        if (source[idx] == 'u' && source[idx + 1] == '8') {
            *type = (types::NumericType *) &types::numeric::u8_instance;
            return idx + 2;
        }

        if (source[idx] == 'i' && source[idx + 1] == '8') {
            *type = (types::NumericType *) &types::numeric::i8_instance;
            return idx + 2;
        }

        // Must now be at least three characters
        if (length < 3) return idx;

        if (source[idx] == 'u' && source[idx + 1] == '1' && source[idx + 2] == '6') {
            *type = (types::NumericType *) &types::numeric::u16_instance;
            return idx + 3;
        }

        if (source[idx] == 'i' && source[idx + 1] == '1' && source[idx + 2] == '6') {
            *type = (types::NumericType *) &types::numeric::i16_instance;
            return idx + 3;
        }

        if (source[idx] == 'u' && source[idx + 1] == '3' && source[idx + 2] == '2') {
            *type = (types::NumericType *) &types::numeric::u32_instance;
            return idx + 3;
        }

        if (source[idx] == 'i' && source[idx + 1] == '3' && source[idx + 2] == '2') {
            *type = (types::NumericType *) &types::numeric::i32_instance;
            return idx + 3;
        }

        if (source[idx] == 'u' && source[idx + 1] == '6' && source[idx + 2] == '4') {
            *type = (types::NumericType *) &types::numeric::u64_instance;
            return idx + 3;
        }

        if (source[idx] == 'i' && source[idx + 1] == '6' && source[idx + 2] == '4') {
            *type = (types::NumericType *) &types::numeric::i64_instance;
            return idx + 3;
        }

//        if (source[idx] == 'f' && source[idx + 1] == '3' && source[idx + 2] == '2') {
//            type = tokens::NumericLiteralType::f32;
//            return idx + 3;
//        }
//
//        if (source[idx] == 'f' && source[idx + 1] == '6' && source[idx + 2] == '4') {
//            type = tokens::NumericLiteralType::f64;
//            return idx + 3;
//        }

        std::string buffer_str("bytes(");
        if (starts_with(source, idx, buffer_str)) {
            idx += (int) buffer_str.length();
            int idx_start = idx;

            while (idx < source.length() && std::isdigit(source[idx]))
                idx++;

            if (idx == idx_start) {
                add_msg(Message::Level::error, "expected digit", idx, 1);
                return idx;
            }

            if (source[idx] != ')') {
                add_msg(Message::Level::error, "mismatched parentheses: expected ')'", idx, 1);
                add_msg(Message::Level::notice, "group opened here", idx_start - 1, 1);

                return idx_start - (int) buffer_str.length();
            }

            std::stringstream stream(source.substr(idx_start, idx - idx_start));
            size_t buf_size;
            stream >> buf_size;

            if (buf_size < 1) {
                add_msg(Message::Level::error, "invalid byte size provided", idx_start, idx - idx_start - 1);
                return idx;
            }

            *type = new types::numeric::Buffer(buf_size);
            return idx + 1;
        }

        std::string type_str("type(");
        if (starts_with(source, idx, type_str)) {
            idx += (int) type_str.length();
            int idx_sym_start = idx;
            int idx_sym_end = parse_symbol_name(source, idx);

            if (idx == idx_sym_end) {
                add_msg(Message::Level::error, "expected symbol", idx, 1);
                return idx;
            }

            idx = idx_sym_end;

            if (source[idx] != ')') {
                add_msg(Message::Level::error, "mismatched parentheses: expected ')'", idx, 1);
                add_msg(Message::Level::notice, "group opened here", idx_sym_start - 1, 1);

                return idx_sym_start - (int) type_str.length();
            }

            // Extract symbol
            std::string symbol = source.substr(idx_sym_start, idx_sym_end - idx_sym_start);

            // Check that symbol is bound
            auto symbol_decl = m_data->m_sdata->get_symbol_declaration(symbol);
            if (symbol_decl == nullptr) {
                add_msg(Message::Level::error, "reference to unbound symbol", idx_sym_start, (int) symbol.length());
                return idx_sym_start - (int) type_str.length();
            }

            // Check type is numeric
            // TODO expand to any type, but note this method handles numeric types only
            auto symbol_type = symbol_decl->get_symbol_type();
            if (!symbol_type->is_numeric()) {
                add_msg(Message::Level::error, "symbol is not numeric", idx_sym_start, (int) symbol.length());
                symbol_decl->add_message(m_data, Message::Level::notice, "symbol declared here");
                return idx_sym_start - (int) type_str.length();
            }

            *type = (types::NumericType*) symbol_type;
            return idx + 1;
        }

        return idx;
    }

    bool Parser::expect_numeric_lit(const std::string &source, int idx) {
        size_t len = source.length();
        return std::isdigit(source[idx]) || (source[idx] == '.' && idx + 1 < len && std::isdigit(source[idx + 1]));
    }

    int Parser::parse_numeric_lit(const std::string &source, int idx_start, tokens::NumericLiteral **lit) {
        int idx = idx_start;
        types::NumericType *type;
        bool explicitly_declared_type = false;
        int radix = 10;

        // Specify numeric literal type
        if (std::isalpha(source[idx])) {
            explicitly_declared_type = true;
            int end = parse_numeric_type(source, idx, &type);

            // No literal; not a number
            if (end == idx || m_data->has_message_of(Message::Level::error)) {
                return idx;
            }

            // Expect colon
            if (source[end] != ':') {
                return idx;
            }

            idx = end + 1;
        }

        // Is the literal negative?
        bool is_neg = source[idx] == '-';

        // Skip sign
        if (source[idx] == '-' || source[idx] == '+') {
            idx++;
        }

        // Get radix
        if (source[idx] == '0' && std::isalpha(source[idx + 1])) {
            radix = get_radix(source[idx + 1]);

            if (radix == -1) {
                add_msg(Message::Level::error, "invalid radix '" + std::string{source[idx + 1]} + "'",
                        idx + 1, 1);
                return idx_start;
            }

            idx += 2;
        }

        // Parse body
        int body_start = idx;
        int body_digit_count = 0;
        while (idx < source.length() && (source[idx] == '_' || IS_BASE_CHAR(source[idx], radix))) {
            idx++;
            if (source[idx] != '_') body_digit_count++;
        }

        // Decimal point?
        if (source[idx] == '.') {
            // TODO
            add_msg(Message::Level::error,
                    "unexpected decimal point (decimals aren't currently supported).", idx, 1);
            return idx_start;
        }

        // Try to store the value
        unsigned long long value = 0; // Base 10 value
        unsigned long long k = 1; // Place multiplying factor
        unsigned long long value_max = is_neg ? LLONG_MAX : ULLONG_MAX;
        bool too_big = false; // Too big? (i.e. must use buffer?)
        int i = idx - 1;

        while (i >= body_start) {
            unsigned long long partial = k * GET_BASE_VAL(source[i], radix);

            // Will this cause an overflow?
            if (value_max - value < partial) {
                too_big = true;
                break;
            }

            value += partial;
            k *= radix;
            i--;
        }

        // If explicitly declared type, ensure we fit, or find a suitable type
        if (explicitly_declared_type) {
            if (too_big) {
                add_msg(Message::Level::error, "numeric literal too large", body_start,
                        idx - body_start);
                return idx_start;
            }

            if (!types::numeric::Integer::does_fit(value, is_neg, (types::numeric::Integer*) type)) {
                add_msg(Message::Level::error,
                        "numeric literal does not fit in the provided type", body_start, idx - body_start);
                return idx_start;
            }
        } else if (!too_big) {
            // Try to find an appropriate type
            if (types::numeric::Integer::does_fit(value, is_neg, (types::numeric::Integer*) &types::numeric::i32_instance)) {
                type = (types::numeric::Integer*) &types::numeric::i32_instance;
            } else if (types::numeric::Integer::does_fit(value, is_neg, (types::numeric::Integer*) &types::numeric::i64_instance)) {
                type = (types::numeric::Integer*) &types::numeric::i64_instance;
            } else if (!is_neg && types::numeric::Integer::does_fit(value, is_neg, (types::numeric::Integer*) &types::numeric::u64_instance)) {
                type = (types::numeric::Integer*) &types::numeric::u64_instance;
            } else {
                too_big = true;
            }
        }

        // Are we too big for conventional numeric types?
        if (too_big) {
            // TODO create correct NumericLiteral to convey this buffer
            add_msg(Message::Level::error, "numeric literal too large (buffers not yet supported)",
                    body_start,
                    idx - body_start);
            return idx_start;
        }

        // Create literal
        delete *lit;
        *lit = new tokens::NumericLiteral(m_data->m_source, m_data->get_row(), idx_start, idx - idx_start, std::to_string(value), type);

        return idx;
    }

    int Parser::parse_symbol_name(const std::string &source, int idx_start) {
        int idx = idx_start;

        // [\$a-zA-Z]
        if (source[idx] != '$' && !std::isalpha(source[idx])) {
            return idx_start;
        }

        idx++;
        size_t len = source.length();

        // [\_a-zA_Z0-9]
        while (idx < len && (source[idx] == '_' || std::isalnum(source[idx])))  {
            idx++;
        }

        return idx;
    }

    int Parser::parse_operator(const std::string &source, int idx, operators::Operator::Type &type) {
        if (source[idx] == '+') {
            type = operators::Operator::Type::addition;
            return idx + 1;
        }

        if (source[idx] == '-') {
            type = operators::Operator::Type::subtraction;
            return idx + 1;
        }

        if (source[idx] == '*') {
            type = operators::Operator::Type::multiplication;
            return idx + 1;
        }

        if (source[idx] == '=') {
            type = operators::Operator::Type::assignment;
            return idx + 1;
        }

        return idx;
    }

    int Parser::parse_expression(const std::string &source, int idx_start, tokens::Token **token_out) {
        if (m_data == nullptr) {
            std::cerr << "m_data is NULL in parse_expression" << std::endl;
            std::exit(1);
        }

        std::vector<tokens::Token *> tokens; // Store list of parsed tokens
        std::stack<tokens::Token *> stack;
        int idx = idx_start;
        size_t len = source.length();

        while (true) {
            tokens::Token *new_token = nullptr;

            // Bracket?
            if (source[idx] == '(') {
                new_token = new tokens::OpenBracketLiteral(m_data->m_source, m_data->get_row(), idx, 1);
                idx++;
                goto loop_end;
            }

            if (source[idx] == ')') {
                new_token = new tokens::ClosedBracketLiteral(m_data->m_source, m_data->get_row(), idx, 1);
                idx++;
                goto loop_end;
            }

            // Operator?
            {
                operators::Operator::Type op_type;
                int idx_end = parse_operator(source, idx, op_type);
                if (idx_end > idx) {
                    auto op = new tokens::BinaryOperator(m_data->m_source, m_data->get_row(), idx, idx_end - idx, op_type);
                    new_token = op;
                    idx = idx_end;
                    goto loop_end;
                }
            }

            // Number?
            if (expect_numeric_lit(source, idx)) {
                tokens::NumericLiteral *num_lit = nullptr;
                idx = parse_numeric_lit(source, idx, &num_lit);

                // Error??
                if (m_data->has_message_of(Message::Level::error)) {
                    goto ret_failure;
                }

                new_token = num_lit;
                goto loop_end;
            }

            // Symbol?
            {
                int idx_end = parse_symbol_name(source, idx);
                if (idx_end != idx) {
                    std::string symbol = source.substr(idx, idx_end - idx);

                    // Check that the symbol exists
                    auto decl = m_data->m_sdata->get_symbol_declaration(symbol);
                    if (decl == nullptr) {
                        add_msg(Message::Level::error, "reference to unbound symbol", idx, idx_end - idx);
                        goto ret_failure;
                    }

                    auto ref = new tokens::SymbolReference(m_data->m_source, m_data->get_row(), idx, idx_end - idx, symbol);
                    ref->set_data(m_data->m_sdata->get_symbol_offset(symbol), decl->get_symbol_type()->size_of());
                    new_token = ref;

                    idx = idx_end;
                    goto loop_end;
                }
            }

            // What?
            add_msg(Message::Level::error, "invalid syntax", idx, 1);
            goto ret_failure;

        loop_end:
            if (new_token == nullptr) {
                std::cerr << "new_token is NULL" << std::endl;
                std::exit(1);
            }

            // Add to stack/tokens acknowledging type and precedence
            auto token_type = new_token->get_token_type();

            if (token_type == tokens::Token::Type::closed_bracket_literal) {
                // ')' - dump stack
                while (!stack.empty() && stack.top()->get_token_type() != tokens::Token::Type::open_bracket_literal) {
                    tokens.push_back(stack.top());
                    stack.pop();
                }

                if (!stack.empty()) stack.pop(); // Remove '('
            } else if (token_type == tokens::Token::Type::open_bracket_literal) {
                // Push '(' directly to the stack
                stack.push(new_token);
            } else if (token_type == tokens::Token::Type::binary_operator) {
                int new_precedence = ((tokens::HasPrecedence*) new_token)->get_precedence();

                // Dump the stack until we can add to it
                while (!stack.empty() && ((tokens::HasPrecedence*) stack.top())->get_precedence() <= new_precedence) {
                    tokens.push_back(stack.top());
                    stack.pop();
                }

                stack.push(new_token);
            } else {
                // Default: just add to tokens vector
                tokens.push_back(new_token);
            }

            // Expect space, or EOL
            if (idx >= len || source[idx] == '\n') {
                break;
            }

            skip_whitespace(source, idx);
        }

        // We have parsed all available tokens - dump remainder of the stack
        while (!stack.empty()) {
            tokens.push_back(stack.top());
            stack.pop();
        }

        // If we have no tokens, exit
        if (tokens.empty()) {
            goto ret_failure;
        }

        // Re-structure into tree
        for (auto token : tokens) {
            auto type = token->get_token_type();

            if (type == tokens::Token::Type::binary_operator) {
                // Check that we have enough arguments
                if (stack.size() < 2) {
                    token->add_message(m_data, Message::Level::error, "binary operator expects 2 arguments, got " + std::to_string(stack.size()));
                    goto ret_failure;
                }

                // Extract arguments BACKWARDS and insert into operator args
                ((tokens::BinaryOperator*) token)->set_right(stack.top());
                stack.pop();
                ((tokens::BinaryOperator*) token)->set_left(stack.top());
                stack.pop();

                // Add operator to stack
                stack.push(token);
            } else {
                // Add to the stack
                stack.push(token);
            }
        }

        // Stack should only contain 1 item. It should NOT be empty.
        if (stack.empty()) {
            std::cerr << "Stack is empty whilst parsing expression" << std::endl;
            std::exit(1);
        } else if (stack.size() > 1) {
            stack.top()->add_message(m_data, Message::Level::error, "expected operator");
            goto ret_failure;
        }

        // Populate output and return
        *token_out = stack.top();
        return idx;

    ret_failure:
        // Clear vector
        for (auto token : tokens) {
            delete token;
        }

        // Dump stack
        while (!stack.empty()) {
            stack.pop();
        }

        *token_out = nullptr;

        return idx_start;
    }

    void Parser::parse(Data *data, const std::string& source_name) {
        data->reset_row();
        data->m_source = source_name;
        data->m_sdata = data->get_source(source_name);
        if (data->m_sdata == nullptr) {
            return;
        }

        m_data = data;

        int idx = 0;
        const std::string source = data->m_sdata->get_contents();
        size_t source_length = source.length();

        // Constants
        std::string decl_str("decl");
        std::string print_str("print");

        while (idx < source_length) {
            int idx_row_start = idx;
            parser::skip_whitespace(source, idx);

            // Is variable declaration?
            if (starts_with(source, idx, decl_str)) {
                idx += (int) decl_str.length();
                parser::skip_whitespace(source, idx);

                while (true) {
                    // Variable name
                    int idx_sym_start = idx;
                    int idx_sym_end = parse_symbol_name(source, idx);
                    if (idx == idx_sym_end) {
                        add_msg(Message::Level::error, "Syntax Error: expected symbol", idx - idx_row_start, 1);
                        return;
                    }

                    // Extract symbol
                    std::string symbol = source.substr(idx_sym_start, idx_sym_end - idx_sym_start);

                    idx = idx_sym_end;
                    parser::skip_whitespace(source, idx);

                    // Colon
                    auto *symbol_type = (types::NumericType*) &types::numeric::i32_instance;
                    if (source[idx] == ':') {
                        parser::skip_whitespace(source, ++idx);

                        // Parse type
                        int idx_type_end = parse_numeric_type(source, idx, &symbol_type);

                        if (m_data->has_message_of(Message::Level::error)) {
                            return;
                        }

                        if (idx_type_end == idx) {
                            int end = idx;
                            parser::skip_alpha(source, end);

                            add_msg(Message::Level::error, "expected literal type specifier", idx,
                                    end - idx);
                            return;
                        }

                        idx = idx_type_end;

                        // Are we shadowing?
                        if (!m_data->options()->allow_shadowing && m_data->m_sdata->get_symbol_scope(symbol) != -1) {
                            add_msg(Message::Level::error, "cannot shadow existing symbol", idx_sym_start,
                                    idx_sym_end - idx_sym_start);

                            auto decl = m_data->m_sdata->get_symbol_declaration(symbol);
                            decl->add_message(m_data, Message::Level::notice, "symbol previously declared here");

                            return;
                        }

                        // Is this a re-declaration? i.e. already exists in topmost scope
                        if (!m_data->options()->allow_symbol_redeclaration && m_data->m_sdata->get_symbol_scope(symbol) == m_data->m_sdata->count_symbol_scopes() - 1) {
                            add_msg(Message::Level::error, "cannot redeclare symbol", idx_sym_start,
                                    idx_sym_end - idx_sym_start);

                            auto decl = m_data->m_sdata->get_symbol_declaration(symbol);
                            decl->add_message(m_data, Message::Level::notice, "symbol previously declared here");

                            return;
                        }

                        parser::skip_whitespace(source, idx);
                    } else {
                        add_msg(Message::Level::notice, "defaulting to i32", idx_sym_start,
                                idx_sym_end - idx_sym_start);
                    }

                    // Add token
                    auto decl = new tokens::SymbolDeclaration(m_data->m_source, m_data->get_row(), idx_sym_start, idx_sym_end - idx_sym_start, symbol, symbol_type);
                    data->m_sdata->add_token(decl);

                    // Record declaration
                    data->m_sdata->insert_symbol(symbol, decl);

                    // Expect comma or EOL
                    if (idx >= source.length() || source[idx] == '\n') {
                        break;
                    } else if (source[idx] == ',') {
                        idx++;
                        skip_whitespace(source, idx);
                        continue;
                    } else {
                        add_msg(Message::Level::error,
                                "Syntax Error: expected comma or EOL, got '" + std::string{source[idx]} + "'",
                                idx - idx_row_start, 1);
                        return;
                    }
                }
            }

            else if (starts_with(source, idx, print_str)) {
                auto print = new tokens::Print(m_data->m_source, m_data->get_row(), idx, (int) print_str.length());
                data->m_sdata->add_token(print);

                idx += (int) print_str.length();
                skip_whitespace(source, idx);

                tokens::Token *token;
                idx = parse_expression(source, idx, &token);
                if (token == nullptr) {
                    return;
                }

                print->set_body(token);
            }

            else {
                // Assume expression
                tokens::Token *token;
                int idx_end = parse_expression(source, idx, &token);
                if (token == nullptr) {
                    return;
                }

                idx = idx_end;
                data->m_sdata->add_token(token);
            }

            // Expect newline or end of input
            if (idx == source_length) {
                break;
            } else if (source[idx] == '\n') {
                idx++;
            } else {
                add_msg(Message::Level::error, "expected end of line", idx - idx_row_start, 1);
            }

            // End
            m_data->inc_row();
        }
    }
}