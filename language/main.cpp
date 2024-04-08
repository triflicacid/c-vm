#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer/Lexer.hpp"

struct Options {
    char *input_file;
    char *output_file;
    char *lexed_file;
    bool debug;

    Options() {
        input_file = nullptr;
        output_file = nullptr;
        lexed_file = nullptr;
        debug = false;
    }
};

/** Parse command-line arguments. */
int parse_arguments(int argc, char **argv, Options &opts) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'd' && !opts.debug) { // Enable debug mode
                opts.debug = true;
            } else if (argv[i][1] == 'o' && !opts.output_file) { // Provide output file
                i++;

                if (i == argc) {
                    std::cout << "-o: expected file path\n";
                    return EXIT_FAILURE;
                }

                opts.output_file = argv[i];
            } else if (argv[i][1] == 'l' && !opts.lexed_file) { // Provide output file
                i++;

                if (i == argc) {
                    std::cout << "-l: expected file path\n";
                    return EXIT_FAILURE;
                }

                opts.lexed_file = argv[i];
            } else {
                std::cout << "Unknown/repeated flag " << argv[i] << "\n";
                return EXIT_FAILURE;
            }
        } else if (!opts.input_file) {
            opts.input_file = argv[i];
        } else {
            std::cout << "Unexpected argument '" << argv[i] << "'\n";
            return EXIT_FAILURE;
        }
    }

    // Check if all files are present
    if (opts.input_file == nullptr) {
        std::cout << "Expected input file to be provided\n";
        return EXIT_FAILURE;
    }

    // TODO output file is not needed atm
//    if (opts.output_file == nullptr && opts.do_compilation) {
//        std::cout << "Expected output file to be provided (-o <file>)\n";
//        return EXIT_FAILURE;
//    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    // Parse provided options
    Options opts;

    if (parse_arguments(argc, argv, opts) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // TODO remove
    opts.debug = true;

    // Read input_file
    std::string file_contents;

    {
        std::ifstream file;
        file.open(opts.input_file, std::ios::in);

        if (!file.is_open()) {
            std::cout << "Unable to open file " << opts.input_file << std::endl;
            return EXIT_FAILURE;
        }

        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        file_contents = stream.str();
    }

    // Create lexer and lex file
    message::List messages;
    language::lexer::Lexer lexer(file_contents);

    lexer.set_path(std::filesystem::canonical(opts.input_file));
    lexer.lex(messages);

    if (message::print_and_check(messages)) {
        return EXIT_FAILURE;
    }

    // Print tokens for reference
    if (opts.lexed_file) {
        std::ofstream file;
        file.open(opts.lexed_file, std::ios::out);

        if (file.is_open()) {
            file << lexer.to_xml();
        } else {
            std::cout << "Unable to open file " << opts.lexed_file << std::endl;
        }

        file.close();
    }

    return EXIT_SUCCESS;
}
