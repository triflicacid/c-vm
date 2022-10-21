#include "src/assembler/assemble.c"

#include <stdio.h>

#include "src/assembler/args.c"
#include "src/assembler/instruction.c"
#include "src/assembler/labels.c"
#include "src/assembler/line.c"
#include "src/assembler/symbol.c"
#include "src/cpu/bit-ops.c"
#include "src/cpu/cpu.c"
#include "src/cpu/opcodes.h"
#include "src/util.c"

int main(int argc, char **argv) {
    char *file_in, *file_out, *file_preproc;
    int is_file_in = 0, is_file_out = 0, preproc = 0, is_file_preproc = 0,
        do_detail = 0, debug = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'd':  // Detail/Debug
                    if (do_detail)
                        debug = 1;
                    else
                        do_detail = 1;
                    break;
                case 'o':  // Out file
                    i++;
                    if (i >= argc) {
                        printf("-o: expected file path\n");
                        return -1;
                    }
                    is_file_out = 1;
                    file_out = argv[i];
                    break;
                case 'p':  // Pre-Processed file
                    preproc = 1;
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        i++;
                        is_file_preproc = 1;
                        file_preproc = argv[i];
                    }
                    break;
                default:
                    printf("Unknown option '%s'\n", argv[i]);
                    return -1;
            }
        } else if (!is_file_in) {
            is_file_in = 1;
            file_in = argv[i];
        } else {
            printf("Unknown argument '%s'\n", argv[i]);
            return -1;
        }
    }

    struct AsmError err = asm_error_create();
    err.print = do_detail;
    err.debug = debug;
    struct AsmData data = asm_data_create();

    // OPEN FILE & SPLIT INTO LINES
    if (do_detail)
        printf("Reading source file '%s'\n",
               is_file_in ? file_in : "source.asm");
    FILE *fp = fopen(is_file_in ? file_in : "source.asm", "r");

    if (debug) printf("=== GET LINES ===\n");
    asm_read_lines(fp, &data, &err);
    if (err.errc) goto end;
    if (debug) {
        printf("--- Source ---\n");
        linked_list_print_AsmLine(data.lines);
    }

    fclose(fp);

    // PRE-PROCESS FILE
    if (debug) printf("=== PRE-PROCESSING ===\n");
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
            printf("Written %u bytes of post-processed source to %s\n", bytes,
                   is_file_preproc ? file_preproc : "preproc.asm");
        free(buf);
    }

    // PARSE LINES
    if (debug) printf("=== PARSING ===\n");
    asm_parse(&data, &err);
    if (err.errc) goto end;
    if (debug) {
        printf("*** BYTES: %llu\n", data.bytes);
        printf("--- Instruction AST ---\n");
        asm_print_instruction_list(data.instructs);
        printf("--- Labels ---\n");
        linked_list_print_AsmLabel(data.labels);
    }

    // COMPILE
    if (debug) printf("=== COMPILE ===\n");
    char *buf = asm_compile(&data, &err);
    if (err.errc) goto end;
    if (debug) printf("> Buffer of size %u bytes at %p.\n", data.bytes, buf);

    if (buf != 0) {
        fp = fopen(is_file_out ? file_out : "source.bin", "w");
        fwrite(buf, data.bytes, 1, fp);
        fclose(fp);
        if (do_detail)
            printf("Written %u bytes to file '%s'\n", data.bytes,
                   is_file_out ? file_out : "source.bin");
        free(buf);
    }

end:
    asm_data_destroy(&data);

    printf("%i", err.errc);

    return err.errc;
}