#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

#include "src/pre-process/pre-processor.hpp"
extern "C" {
#include "util.h"
}
#include "assembler_data.hpp"
#include "parser.hpp"

struct Options {
    char *input_file;
    char *output_file;
    char *post_processing_file;
    bool debug;
    bool strict_sections;
    bool do_compilation;
    bool do_pre_processing;

    Options() {
        input_file = nullptr;
        output_file = nullptr;
        post_processing_file = nullptr;
        debug = false;
        strict_sections = false;
        do_compilation = true;
        do_pre_processing = true;
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
            } else if (argv[i][1] == 'p' && !opts.post_processing_file) { // Provide post-process output file
                i++;

                if (i >= argc) {
                    std::cout << "-p: expected file path\n";
                    return EXIT_FAILURE;
                }

                opts.post_processing_file = argv[i];
            } else if (opts.do_pre_processing && strcmp(argv[i] + 1, "-no-pre-process") == 0) { // Skip pre-processing
                opts.do_pre_processing = false;
            } else if (opts.do_compilation && strcmp(argv[i] + 1, "-no-compile") == 0) { // Skip compilation
                opts.do_compilation = false;
            } else if (!opts.strict_sections && strcmp(argv[i] + 1, "-strict-sections") == 0) {
                opts.strict_sections = true;
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

    if (opts.output_file == nullptr && opts.do_compilation) {
        std::cout << "Expected output file to be provided (-o <file>)\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/** Pre-process the given data, write to file if not NULL. */
int pre_process_data(assembler::pre_processor::Data& data, message::List& messages, char *output_file) {
    if (data.debug)
        std::cout << CONSOLE_GREEN "=== PRE-PROCESSING ===\n" CONSOLE_RESET;

    assembler::pre_process(data, messages);

    // Check if error
    if (message::print_and_check(messages))
        return EXIT_FAILURE;

    if (data.debug) {
        // Print constants
        std::cout << "--- Constants ---\n";

        for (const auto& pair : data.constants) {
            std::cout << "%define " << pair.first << " " << pair.second.value << "\n";
        }

        // Print macros
        std::cout << "--- Macros ---\n";

        for (const auto& pair : data.macros) {
            std::cout << "%macro " << pair.first << " ";

            for (const auto& param : pair.second.params) {
                std::cout << param << " ";
            }

            std::cout << "(" << pair.second.lines.size() << " lines)\n";
        }
    }

    // Write post-processed content to file?
    if (output_file) {
        std::ofstream file(output_file);

        // If file failed to open, this is bad but NOT fatal
        if (!file.good()) {
            std::cout << "Failed to open file " << output_file << "\n";
            return EXIT_SUCCESS;
        }

        // Write post-processed content to the output stream
        std::string content = data.write_lines();
        file << content;
        file.close();

        if (data.debug)
            std::cout << "Written " << content.size() << " bytes of post-processed source to " << output_file << "\n";
    }

    return EXIT_SUCCESS;
}

/** Parse the given data. */
int parse_data(assembler::Data &data, message::List& messages) {
    // Parse pre-processed lines
    if (data.debug)
        std::cout << CONSOLE_GREEN "=== PARSING ===\n" CONSOLE_RESET;

    assembler::parser::parse(data, messages);

    // Check if error
    if (message::print_and_check(messages)) {
        return EXIT_FAILURE;
    }

    if (data.debug) {
        // Print chunks
        std::cout << "--- Chunks ---\n";

        for (const auto& chunk : data.chunks) {
            chunk->print();
        }
    }

    return EXIT_SUCCESS;
}

/** Compile data to given file. */
int compile_result(assembler::Data& data, char *output_file) {
    // Open output file
    std::ofstream file(output_file, std::ios::binary);

    // Check if the file exists
    if (!file.good()) {
        std::cout << "Failed to open output file " << output_file << "\n";
        return EXIT_FAILURE;
    }

    // Write compiled chunks to output file
    auto before = file.tellp();
    data.write_headers(file);
    data.write_chunks(file);
    auto after = file.tellp();

    if (data.debug)
        std::cout << "Written " << (after - before + 1) << " bytes to file " << output_file << "\n";

    file.close();

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // Parse CLI
    Options opts;

    if (parse_arguments(argc, argv, opts) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // Set-up pre-processing data
    assembler::pre_processor::Data pre_data(opts.debug);
    pre_data.set_executable(argv[0]);
    message::List messages;

    // Read source file into lines
    if (opts.debug)
        std::cout << "Reading source file '" << opts.input_file << "'\n";

    assembler::read_source_file(opts.input_file, pre_data, messages);

    // Check if error
    if (message::print_and_check(messages))
        return EXIT_FAILURE;

    // Pre-process file
    if (opts.do_pre_processing && pre_process_data(pre_data, messages, opts.post_processing_file) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // Construct data structure for parsing
    assembler::Data data(pre_data);
    data.strict_sections = opts.strict_sections;

    if (parse_data(data, messages) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // Compile data
    if (opts.do_compilation && compile_result(data, opts.output_file) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}