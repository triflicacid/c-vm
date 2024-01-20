#define DEBUG_MODE false

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "src/assemble.h"

int main(int argc, char **argv) {
    char *file_in = NULL, *file_out = NULL, *file_preproc;
    bool is_file_in = 0, is_file_out = 0, preproc = 0, is_file_preproc = 0,
        do_detail = DEBUG_MODE, debug = DEBUG_MODE;
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
                    is_file_out = true;
                    file_out = argv[i];
                    break;
                case 'p':  // Pre-Processed file
                    preproc = 1;
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        i++;
                        is_file_preproc = true;
                        file_preproc = argv[i];
                    }
                    break;
                default:
                    printf("Unknown option '%s'\n", argv[i]);
                    return EXIT_FAILURE;
            }
        } else if (!is_file_in) {
            is_file_in = true;
            file_in = argv[i];
        } else {
            printf("Unknown argument '%s'\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    if (!is_file_in) {
        printf("Expected input file to be provided\n");
        return EXIT_FAILURE;
    }

    struct AsmError err = asm_error_create();
    err.print = do_detail;
    err.debug = debug;
    struct AsmData data = asm_data_create();

    // OPEN FILE & SPLIT INTO LINES
    if (do_detail)
        printf("Reading source file '%s'\n", file_in);
    FILE *fp = fopen(file_in, "r");

    if (debug) printf(CONSOLE_GREEN "=== GET LINES ===\n" CONSOLE_RESET);
    asm_read_lines(fp, &data, &err);
    if (err.errc) goto end;
    if (debug) {
        printf("--- Source ---\n");
        linked_list_print_AsmLine(data.lines);
    }

    fclose(fp);

    // PRE-PROCESS FILE
    if (debug) printf(CONSOLE_GREEN "=== PRE-PROCESSING ===\n" CONSOLE_RESET);
    asm_preprocess(&data, &err);
    if (err.errc) goto end;
    if (debug) {
        printf("--- Symbols ---\n");
        linked_list_print_AsmSymbol(data.symbols);
        printf("--- Source ---\n");
        linked_list_print_AsmLine(data.lines);
    }
    if (preproc) {
        char *buf;
        unsigned long long bytes = asm_write_lines(data.lines, &buf);
        --bytes;  // Do not copy null character
        fp = fopen(is_file_preproc ? file_preproc : "preproc.asm", "w");
        fwrite(buf, bytes, 1, fp);
        fclose(fp);
        if (do_detail)
            printf("Written %llu bytes of post-processed source to %s\n", bytes,
                   is_file_preproc ? file_preproc : "preproc.asm");
        free(buf);
    }

    // PARSE LINES
    if (debug) printf(CONSOLE_GREEN "=== PARSING ===\n" CONSOLE_RESET);
    asm_parse(&data, &err);
    if (err.errc) goto end;
    if (debug) {
        printf("--- Labels ---\n");
        linked_list_print_AsmLabel(data.labels);
        printf("--- Chunks ---\n");
        linked_list_print_AsmChunk(data.chunks);
        printf("Program Head: %llu bytes.\n- Execution start offset: +%08llX\n", sizeof(data.head_data), data.head_data.start_addr);
        printf("Program Body: %llu bytes\n", data.bytes);
    }

    // COMPILE
    if (debug) printf(CONSOLE_GREEN "=== COMPILE ===\n" CONSOLE_RESET);
    char *buf = NULL;
    size_t buf_size = asm_compile(&data, &err, &buf);
    if (err.errc) goto end;
    if (debug) printf("> Buffer of size %llu bytes at %p.\n", buf_size, buf);

    if (buf != NULL) {
        fp = fopen(is_file_out ? file_out : "source.bin", "wb");
        fwrite(buf, buf_size, 1, fp);
        fclose(fp);
        if (do_detail)
            printf("Written %llu bytes to file '%s'\n", buf_size,
                   is_file_out ? file_out : "source.bin");
        free(buf);
    }

end:
    asm_data_destroy(&data);

    printf("%i", err.errc);

    return err.errc;
}