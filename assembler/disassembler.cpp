#include <iostream>
#include <fstream>
#include <cstring>

#include "disassembler_data.hpp"
#include "disassembler.hpp"
#include "messages/list.hpp"

struct Options {
    char *input_file;
    char *output_file;
    bool debug;
    bool format_data;
    bool no_labels;
    bool no_commas;

    Options() {
        input_file = nullptr;
        output_file = nullptr;
        debug = false;
        format_data = false;
        no_labels = false;
        no_commas = false;
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
            } else if (!opts.format_data && strcmp(argv[i] + 1, "-format-data") == 0) {
                opts.format_data = true;
            } else if (!opts.no_labels && strcmp(argv[i] + 1, "-no-labels") == 0) {
                opts.no_labels = true;
            } else if (!opts.no_commas && strcmp(argv[i] + 1, "-no-commas") == 0) {
                opts.no_commas = true;
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

/** Write assembly to file. */
int write_result(disassembler::Data& data, char *output_file) {
    // Open output file
    std::ofstream file(output_file, std::ios::binary);

    // Check if the file exists
    if (!file.good()) {
        std::cout << "Failed to open output file " << output_file << "\n";
        return EXIT_FAILURE;
    }

    // Write compiled chunks to output file
    auto before = file.tellp();
    file << data.assembly.str();
    auto after = file.tellp();

    if (data.debug)
        std::cout << "Written " << (after - before + 1) << " bytes to file " << output_file << "\n";

    file.close();

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
    data.format_data = opts.format_data;
    data.insert_labels = !opts.no_labels;
    data.insert_commas = !opts.no_commas;

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
    if (opts.debug)
        std::cout << "Disassembling binary...\n";

    message::List messages;
    disassembler::disassemble(data, messages);

    if (handle_messages(messages)) {
        return EXIT_FAILURE;
    }

    // Write to output file
    if (write_result(data, opts.output_file) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
