#include "Token.hpp"

namespace language::lexer {
    std::map<Token::Type, std::string> tokens = {
            { Token::Type::KW_DECL, "decl" },
            { Token::Type::KW_FUNC, "func" },

            { Token::Type::TYPE_u8, "u8" },
            { Token::Type::TYPE_i8, "i8" },
            { Token::Type::TYPE_u16, "u16" },
            { Token::Type::TYPE_i16, "i16" },
            { Token::Type::TYPE_u32, "u32" },
            { Token::Type::TYPE_i32, "i32" },
            { Token::Type::TYPE_u64, "u64" },
            { Token::Type::TYPE_i64, "i64" },

            { Token::Type::LPARENS, "(" },
            { Token::Type::RPARENS, ")" },
            { Token::Type::LBRACKET, "[" },
            { Token::Type::RBRACKET, "]" },
            { Token::Type::LBRACE, "{" },
            { Token::Type::RBRACE, "}" },

            { Token::Type::COLON, ":" },
            { Token::Type::ARROW, "->" },
            { Token::Type::COMMA, "," },
            { Token::Type::EQUALS, "=" },
            { Token::Type::PLUS, "+" },
            { Token::Type::DASH, "-" },
            { Token::Type::STAR, "*" },
            { Token::Type::SLASH, "/" },
    };
}
