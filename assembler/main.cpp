#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

#include "src/pre-process/pre-processor.hpp"
#include "src/messages/list.hpp"
extern "C" {
#include "util.h"
}
#include "data.hpp"
#include "parser.hpp"

/** Handle message list: print messages and empty the list, return if there was an error. */
bool handle_messages(assembler::message::List& list) {
    list.for_each_message([] (assembler::message::Message &msg) {
        msg.print();
    });

    bool is_error = list.has_message_of(assembler::message::Level::Error);

    list.clear();

    return is_error;
}

int main(int argc, char **argv) {
    char *file_in = nullptr, *file_out = nullptr, *file_postproc = nullptr;
    bool debug = false;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'd':  // Print debug info
                    debug = true;
                    break;
                case 'o':  // Out file
                    i++;
                    if (i >= argc) {
                        std::cout << "-o: expected file path\n";
                        return EXIT_FAILURE;
                    }
                    file_out = argv[i];
                    break;
                case 'p':  // Pre-Processed file
                    i++;
                    if (i >= argc) {
                        std::cout << "-p: expected file path\n";
                        return EXIT_FAILURE;
                    }
                    file_postproc = argv[i];
                    break;
                default:
                    std::cout << "Unknown option '" << argv[i] << "'\n";
                    return EXIT_FAILURE;
            }
        } else if (file_in == nullptr) {
            file_in = argv[i];
        } else {
            std::cout << "Unknown argument '" << argv[i] << "'\n";
            return EXIT_FAILURE;
        }
    }

    // Check if all files are present
    if (file_in == nullptr) {
        std::cout << "Expected input file to be provided\n";
        return EXIT_FAILURE;
    }

    if (file_out == nullptr) {
        std::cout << "Expected output file to be provided (-o flag)\n";
        return EXIT_FAILURE;
    }

    // Set-up pre-processing data
    assembler::pre_processor::Data pre_data(debug);
    assembler::message::List messages;

    // Read source file into lines
    if (debug)
        std::cout << "Reading source file '" << file_in << "'\n";

    assembler::read_source_file(file_in, pre_data, messages);

    // Check if error
    if (handle_messages(messages))
        return EXIT_FAILURE;

    // Pre-process file
    if (debug)
        std::cout << CONSOLE_GREEN "=== PRE-PROCESSING ===\n" CONSOLE_RESET;

    assembler::pre_process(pre_data, messages);

    // Check if error
    if (handle_messages(messages))
        return EXIT_FAILURE;

    if (debug) {
        // Print constants
        std::cout << "--- Constants ---\n";

        for (const auto& pair : pre_data.constants) {
            std::cout << "%define " << pair.first << " " << pair.second.value << "\n";
        }

        // Print macros
        std::cout << "--- Macros ---\n";

        for (const auto& pair : pre_data.macros) {
            std::cout << "%macro " << pair.first << " ";

            for (const auto& param : pair.second.params) {
                std::cout << param << " ";
            }

            std::cout << "(" << pair.second.lines.size() << " lines)\n";
        }
    }

    // Write post-processed content to file?
    if (file_postproc) {
        std::ofstream file(file_postproc);

        if (!file.good()) {
            if (pre_data.debug) {
                std::cout << "Failed to open file " << file_postproc << "\n";
            }

            return EXIT_FAILURE;
        }

        // Write post-processed content to the output stream
        std::string content = pre_data.write_lines();
        file << content;
        file.close();

        if (debug)
            std::cout << "Written " << content.size() << " bytes of post-processed source to " << file_postproc << "\n";
    }

    // Construct data structure for parsing
    assembler::Data data(pre_data);

    // Parse pre-processed lines
    if (debug)
        std::cout << CONSOLE_GREEN "=== PARSING ===\n" CONSOLE_RESET;

    assembler::parser::parse(data, messages);

    // Check if error
    if (handle_messages(messages)) {
        return EXIT_FAILURE;
    }

    if (debug) {
        // Print chunks
        std::cout << "--- Chunks ---\n";

        for (const auto& chunk : data.chunks) {
            chunk->print();
        }
    }

    // Open output file
    std::ofstream file(file_out, std::ios::binary);

    // Check if the file exists
    if (!file.good()) {
        std::cout << "Failed to open output file " << file_out << "\n";
        return EXIT_FAILURE;
    }

    // Write compiled chunks to output file
    auto before = file.tellp();
    data.write_headers(file);
    data.write_chunks(file);
    auto after = file.tellp();

    if (debug)
        std::cout << "Written " << (after - before + 1) << " bytes to file " << file_out << "\n";

    file.close();

    return EXIT_SUCCESS;
}