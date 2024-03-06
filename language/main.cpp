#include <fstream>
#include <sstream>
#include "parser/Data.h"
#include "parser/Parser.h"

int main() {
    std::string file_path("../program");

    // Setup data
    language::parser::Data data;
    data.add_source(file_path);
    data.get_source(file_path)->load_from_file();

    // Configure the options
//    data.options()->allow_symbol_redeclaration = false;

    // Parse the source
    language::parser::Parser p;
    p.parse(&data, file_path);

    // Any messages?
    if (data.count_message()) {
        data.for_each_message([](language::Message &message) {
            message.print();
        });
    }

    // Was there an error?
    if (data.has_message_of(language::Message::Level::error)) {
        return EXIT_FAILURE;
    }

    data.get_source(file_path)->debug_print_tokens();

    data.build();

    return EXIT_SUCCESS;
}

