#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

#include "src/pre-process/pre-processor.h"
#include "util.h"

int main(int argc, char **argv) {
    char *file_in = nullptr, *file_out = nullptr, *file_postproc = nullptr;
    bool do_detail = false, debug = false;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'd':  // Detail/Debug
                    if (do_detail)
                        debug = true;
                    else
                        do_detail = true;
                    break;
                case 'o':  // Out file
                    i++;
                    if (i >= argc) {
                        printf("-o: expected file path\n");
                        return EXIT_FAILURE;
                    }
                    file_out = argv[i];
                    break;
                case 'p':  // Pre-Processed file
                    i++;
                    if (i >= argc) {
                        printf("-p: expected file path\n");
                        return EXIT_FAILURE;
                    }
                    file_postproc = argv[i];
                    break;
                default:
                    printf("Unknown option '%s'\n", argv[i]);
                    return EXIT_FAILURE;
            }
        } else if (file_in == nullptr) {
            file_in = argv[i];
        } else {
            printf("Unknown argument '%s'\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    // Check if all files are present
    if (file_in == nullptr) {
        printf("Expected input file to be provided\n");
        return EXIT_FAILURE;
    }

    if (file_out == nullptr) {
        printf("Expected output file to be provided (-o flag)\n");
        return EXIT_FAILURE;
    }

    // Set-up pre-processing data
    assembler::pre_processor::Data data(debug);
    assembler::Error *err = nullptr;

    // Read source file into lines
    if (do_detail)
        printf("Reading source file '%s'\n", file_in);

    assembler::read_source_file(file_in, &data, &err);

    if (err) {
        if (do_detail)
            err->print();

        goto end;
    }

    // Pre-process file
    if (debug)
        printf(CONSOLE_GREEN "=== PRE-PROCESSING ===\n" CONSOLE_RESET);

    assembler::pre_process(&data, &err);

    if (err) {
        if (do_detail)
            err->print();

        goto end;
    }

    if (debug) {
        // Print constants
        std::cout << "--- Constants ---\n";

        for (const auto& pair : data.constants) {
            std::cout << pair.first << " = '" << pair.second << "'\n";
        }
    }

    // Write post-processed content to file?
    if (file_postproc) {
        std::ofstream file(file_postproc);

        if (!file.good()) {
            if (data.debug) {
                std::cout << "Failed to open file " << file_postproc << "\n";
            }

            goto end;
        }

        // Write post-processed content to the output stream
        std::string content = data.write_lines();
        file << content;
        file.close();

        if (data.debug) {
            std::cout << "Written " << content.size() << " bytes of post-processed source to " << file_postproc << "\n";
        }
    }

//    // PARSE LINES
//    if (debug) printf(CONSOLE_GREEN "=== PARSING ===\n" CONSOLE_RESET);
//    asm_parse(&data, &err);
//    if (err.errc) goto end;
//    if (debug) {
//        printf("--- Labels ---\n");
//        linked_list_print_AsmLabel(data.labels);
//        printf("--- Chunks ---\n");
//        linked_list_print_AsmChunk(data.chunks);
//        printf("Program Head: %llu bytes.\n- Execution start offset: +%08llX\n", sizeof(data.head_data), data.head_data.start_addr);
//        printf("Program Body: %llu bytes\n", data.bytes);
//    }
//
//    // COMPILE
//    if (debug) printf(CONSOLE_GREEN "=== COMPILE ===\n" CONSOLE_RESET);
//    char *buf = NULL;
//    size_t buf_size = asm_compile(&data, &err, &buf);
//    if (err.errc) goto end;
//    if (debug) printf("> Buffer of size %llu bytes at %p.\n", buf_size, buf);
//
//    if (buf != NULL) {
//        fp = fopen(is_file_out ? file_out : "source.bin", "wb");
//        fwrite(buf, buf_size, 1, fp);
//        fclose(fp);
//        if (do_detail)
//            printf("Written %llu bytes to file '%s'\n", buf_size,
//                   is_file_out ? file_out : "source.bin");
//        free(buf);
//    }

end:
    int code = 0;

    if (err) {
        code = err->get_type();
        delete err;
    }

    if (do_detail) {
        printf("Terminated with code %i\n", code);
    }
    else {
        printf("%i", code);
    }

    return code;
}