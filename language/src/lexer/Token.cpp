#include "Token.hpp"

namespace language::lexer {
    std::map<Token::Type, std::string> tokens = {
            { Token::KW_DECL, "decl" },
            { Token::KW_DATA, "data" },
            { Token::KW_FUNC, "func" },

            { Token::TYPE_u8, "u8" },
            { Token::TYPE_i8, "i8" },
            { Token::TYPE_u16, "u16" },
            { Token::TYPE_i16, "i16" },
            { Token::TYPE_u32, "u32" },
            { Token::TYPE_i32, "i32" },
            { Token::TYPE_u64, "u64" },
            { Token::TYPE_i64, "i64" },

            { Token::UNIT, "()" },

            { Token::LPARENS, "(" },
            { Token::RPARENS, ")" },
            { Token::LBRACKET, "[" },
            { Token::RBRACKET, "]" },
            { Token::LBRACE, "{" },
            { Token::RBRACE, "}" },

            { Token::COLON, ":" },
            { Token::ARROW, "->" },
            { Token::COMMA, "," },
            { Token::EQUALS, "=" },
            { Token::PLUS, "+" },
            { Token::DASH, "-" },
            { Token::STAR, "*" },
            { Token::SLASH, "/" },
    };

    std::vector<Token::Type> valid_types = {
            Token::TYPE_u8,
            Token::TYPE_i8,
            Token::TYPE_u16,
            Token::TYPE_i16,
            Token::TYPE_u32,
            Token::TYPE_i32,
            Token::TYPE_u64,
            Token::TYPE_i64,
            Token::DATA_IDENTIFIER,
    };

    std::map<Token::Type, NumInfo> num_types = {
            { Token::TYPE_u8, { 8, false, false } },
            { Token::TYPE_i8, { 8, true, false } },
            { Token::TYPE_u16, { 16, false, false } },
            { Token::TYPE_i16, { 16, true, false } },
            { Token::TYPE_u32, { 32, false, false } },
            { Token::TYPE_i32, { 32, true, false } },
            { Token::TYPE_u64, { 64, true, false } },
            { Token::TYPE_i64, { 64, false, false } }
    };

    std::string Token::repr(Token::Type type) {
        switch (type) {
            case Token::DATA_IDENTIFIER:
                return "data identifier";
            case Token::IDENTIFIER:
                return "identifier";
            case Token::INTEGER:
                return "integer";
            case Token::EOL:
                return "<EOL>";
            default:
                return "\"" + tokens.find(type)->second + "\"";
        }
    }
}