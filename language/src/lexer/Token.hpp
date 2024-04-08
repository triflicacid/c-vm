#pragma once

#include <map>
#include <utility>

#include "Location.hpp"

namespace language::lexer {
    /** This class describes a basic token. */
    class Token {
    public:
        enum Type {
            // Special marker to indicate end of a line
            EOL = -1,

            KW_DECL,  // KEYWORD START
            KW_FUNC,  // KEYWORD END

            TYPE_u8, // TYPE START
            TYPE_i8,
            TYPE_u16,
            TYPE_i16,
            TYPE_u32,
            TYPE_i32,
            TYPE_u64,
            TYPE_i64, // TYPE END

            LPARENS, // "("  BRACKET START
            RPARENS, // ")"
            LBRACKET, // "["
            RBRACKET, // "]"
            LBRACE, // "{"
            RBRACE, // "}"  BRACKET END

            COLON, // ":"
            ARROW, // "->"

            COMMA, // ","  OPERATOR START
            PLUS, // "+"
            EQUALS, // "="
            DASH, // "-"
            STAR, // "*"
            SLASH, // "/"  OPERATOR END

            INTEGER, // [0-9]+   LITERALS START
            SYMBOL_NAME, // [a-z]+   LITERALS END

            DATA_NAME, // [A-Z][a-z]*
        };

    private:
        Type m_type;
        Location m_loc;
        std::string m_string;

    public:
        Token(Location loc, Type type, std::string string) : m_loc(loc), m_type(type), m_string(std::move(string)) {};

        Type type() { return m_type; }

        std::string image() { return m_string; }

        Location location() { return m_loc; }
    };

    /** Is 'u8', 'i8', ... */
    inline bool is_type(Token::Type t) {
        return t >= Token::Type::TYPE_u8 && t <= Token::Type::TYPE_i64;
    }

    /** Is a keyword. */
    inline bool is_keyword(Token::Type t) {
        return t >= Token::Type::KW_DECL && t <= Token::Type::KW_FUNC;
    }

    /** Is any type of bracket. */
    inline bool is_bracket(Token::Type t) {
        return t >= Token::Type::LPARENS && t <= Token::Type::RBRACE;
    }

    /** Is operator. */
    inline bool is_operator(Token::Type t) {
        return t >= Token::Type::COMMA && t <= Token::Type::SLASH;
    }

    /** Is a literal? */
    inline bool is_literal(Token::Type t) {
        return t >= Token::Type::INTEGER && t <= Token::Type::SYMBOL_NAME;
    }

    /** Return whether we expect something after this token. */
    inline bool expect_after(Token::Type t) {
        return t == Token::Type::LPARENS
            || t == Token::Type::LBRACKET
            || t == Token::Type::LBRACE
            || is_operator(t);
    }

    /** Map of token types to pre-requisite string. */
    extern std::map<Token::Type, std::string> tokens;
}
