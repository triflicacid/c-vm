#include <iostream>
#include <fstream>

#include "disassembler_data.hpp"
#include "disassembler.hpp"
#include "messages/list.hpp"

struct Options {
    char *input_file;
    char *output_file;
    bool debug;

    Options() {
        input_file = nullptr;
        output_file = nullptr;
        debug = false;
    }
};

/** Handle message list: print messages and empty the list, return if there was an error. */
bool handle_messages(message::List& list) {
    list.for_each_message([] (message::Message &msg) {
        msg.print();
    });

    bool is_error = list.has_message_of(message::Level::Error);

    list.clear();

    return is_error;
}

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

    if (opts.output_file == nullptr) {
        std::cout << "Expected output file to be provided (-o <file>)\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // Parse arguments
    Options opts;

    if (parse_arguments(argc, argv, opts) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // Read input file
    if (opts.debug)
        std::cout << "Reading source file '" << opts.input_file << "'... ";

    disassembler::Data data(opts.debug);

    if (data.load_binary_file(opts.input_file)) {
        if (opts.debug)
            std::cout << "Read " << data.buffer_size << " bytes.\n";
    } else {
        if (opts.debug)
            std::cout << "Cannot open file.\n";
        else
            std::cout << "Failed to read file " << opts.input_file << "\n";

        return EXIT_FAILURE;
    }

    // Disassemble
    message::List messages;

    disassembler::disassemble(data, messages);

    if (handle_messages(messages)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
