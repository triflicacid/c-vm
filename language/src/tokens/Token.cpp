#include "Token.h"
#include "NumericLiteral.h"
#include "parser/Data.h"
#include "Message.h"
#include "parser/util.h"
#include <iostream>

namespace language::tokens {
    void Token::debug_print(int indent) {
        if (indent > 0) std::cout << std::string(indent, ' ');
        _debug_print(indent);
    }

    void Token::_debug_print(int indent) {
        std::cout << "Token\n";
    }

    void Token::add_message(parser::Data *data, Message::Level level, std::string msg) {
        parser::SourceData *src_data = data->get_source(src_name);
        if (src_data == nullptr) {
            std::cerr << "src_data is NULL -- cannot find source with name '" << src_name << "'" << std::endl;
            std::exit(1);
        }

        int index = src_idx;
        std::string err_string = parser::extract_substr_between_newlines(src_data->get_contents(), index, src_len);

        Message message(level, src_row, index, index, src_len, src_data->get_path(), std::move(msg), err_string);
        data->add_message(message);
    }
}
