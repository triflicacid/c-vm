#pragma once

#include <map>
#include <utility>
#include <vector>

#include "Location.hpp"

namespace language::lexer {
    /** This class describes a basic token. */
    class Token {
    public:
        enum Type {
            // Special marker to indicate end of a line
            EOL = -1,

            KW_DECL,  // KEYWORD START
            KW_DATA,
            KW_ENTRY,
            KW_RETURN,
            KW_FUNC,  // KEYWORD END

            TYPE_u8, // TYPE START
            TYPE_i8,
            TYPE_u16,
            TYPE_i16,
            TYPE_u32,
            TYPE_i32,
            TYPE_u64,
            TYPE_i64, // TYPE END

            UNIT, // "()", special case of brackets

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
            IDENTIFIER, // [a-z]+   LITERALS END

            DATA_IDENTIFIER, // [A-Z][a-z]*
        };

    private:
        Type m_type;
        Location m_loc;
        std::string m_string;

    public:
        Token(Location loc, Type type, std::string string) : m_loc(loc), m_type(type), m_string(std::move(string)) {};

        [[nodiscard]] Type type() const { return m_type; }

        [[nodiscard]] const std::string& image() const { return m_string; }

        [[nodiscard]] size_t size() const { return m_string.size(); }

        [[nodiscard]] Location location() const { return m_loc; }

        /** Get token string representation. */
        static std::string repr(Type type);

        /** Is any type of bracket. */
        static bool is_bracket(Type t) {
            return t >= LPARENS && t <= RBRACE;
        }

        /** Is any type of opening bracket. */
        static bool is_opening_bracket(Type t) {
            return t == LPARENS || t == LBRACKET || t == LBRACE;
        }

        /** Is any type of closing bracket. */
        static bool is_closing_bracket(Type t) {
            return t == RPARENS || t == RBRACKET || t == RBRACE;
        }

        /** Check if t2 is the closing bracket fo t1. */
        static bool is_bracket_pair(Type t1, Type t2) {
            return t1 + 1 == t2;
        }

        /** Is operator. */
        static bool is_operator(Type t) {
            return t >= COMMA && t <= SLASH;
        }
    };

    /** Map of token types to pre-requisite string. */
    extern std::map<Token::Type, std::string> tokens;

    /** Vector of numerical type tokens. */
    extern std::vector<Token::Type> num_types;
}
