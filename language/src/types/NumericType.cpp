#include "NumericType.hpp"

namespace language::types {
    std::map<lexer::Token::Type, NumericType> num_types = {
            { lexer::Token::TYPE_u8, NumericType{ lexer::Token::TYPE_u8, 8, false, false } },
            { lexer::Token::TYPE_i8, NumericType{ lexer::Token::TYPE_i8, 8, true, false } },
            { lexer::Token::TYPE_u16, NumericType{ lexer::Token::TYPE_u16, 16, false, false } },
            { lexer::Token::TYPE_i16, NumericType{ lexer::Token::TYPE_i16, 16, true, false } },
            { lexer::Token::TYPE_u32, NumericType{ lexer::Token::TYPE_u32, 32, false, false } },
            { lexer::Token::TYPE_i32, NumericType{ lexer::Token::TYPE_i32, 32, true, false } },
            { lexer::Token::TYPE_u64, NumericType{ lexer::Token::TYPE_u64, 64, false, false } },
            { lexer::Token::TYPE_i64, NumericType{ lexer::Token::TYPE_i64, 64, true, false } },
    };

    lexer::Token::Type NumericType::get_suitable_type(unsigned long long int value) {
        if (value <= CHAR_MAX) return lexer::Token::TYPE_i8;
        if (value <= SHRT_MAX) return lexer::Token::TYPE_i16;
        if (value <= INT_MAX) return lexer::Token::TYPE_i32;
        if (value <= LLONG_MAX) return lexer::Token::TYPE_i64;
        return lexer::Token::TYPE_u64;
    }

    bool can_implicitly_cast_to(const Type *fst, const Type *snd) {
        return fst && snd && fst->category() == Category::Numeric && snd->category() == Category::Numeric
               && NumericType::can_implicitly_cast_to((NumericType *) fst, (NumericType *) snd);
    }
}
