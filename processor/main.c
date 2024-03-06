#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "bit-ops.h"
#include "cpu.h"
#include "binary_header.h"

int main(int argc, char **argv) {
    char *file_in, *file_out;
    bool is_file_in = 0, is_file_out = 0, do_detail = 0;
    WORD_T mem_size = 0xFFF, stack_size = 0x1FF;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'd':  // Print detail
                    do_detail = true;
                    break;
                case 'm':  // Memory size
                    i++;
                    if (i >= argc) {
                        printf("-m: expected number\n");
                        return EXIT_FAILURE;
                    }
                    mem_size = strtoll(argv[i], (char **)(argv[i] + strlen(argv[0])), 10);
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
                case 's':  // Stack size
                    i++;
                    if (i >= argc) {
                        printf("-s: expected number\n");
                        return EXIT_FAILURE;
                    }

                    stack_size = strtoll(argv[i], (char **)(argv[i] + strlen(argv[0])), 10);
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

    // Create CPU and set stack size
    CPU cpu = cpu_create(mem_size);
    cpu_set_stack_size(cpu, stack_size);

    // Set CPUs output file
    FILE *fout = NULL;
    if (is_file_out) {
        if (do_detail) printf("Set STDOUT='%s'\n", file_out);
        fout = fopen(file_out, "w");
        cpu_set_fout(cpu, fout);
    }

    if (do_detail) {
        printf("Initialising CPU...\n");
        cpu_print_details(cpu);
        printf("\n");
    }

    // Read binary source file
    FILE *fp = fopen((is_file_in ? file_in : "program"), "rb");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);
    if (do_detail)
        printf("Reading source file '%s' (%li bytes)... ", (is_file_in ? file_in : "source.bin"), fsize);

    // Read header
    struct binary_header_data header = {0};
    fread(&header, sizeof(header), 1, fp);
    if (do_detail) {
        printf("Loaded header (%llu bytes).\n- Execution start offset: +%08llX\n", sizeof(header), header.start_addr);
    }

    // Move to program body
    if (fseek(fp, (long) sizeof(header), SEEK_SET) != 0) {
        printf("Error: failed to seek in file; binary corrupted.\n");
        return EXIT_FAILURE;
    }

    // Load program into memory
    cpu_load_file_into_mem(cpu, fp, 0, fsize - sizeof(header));
    fclose(fp);

    if (do_detail)
        printf("Done.\n\n");

    // Load program header
    cpu_load_header(cpu, &header);

    // Run fetch-execute cycle
    cpu_fetch_execute_cycle(cpu);

    // Dispose of resources
    cpu_destroy(cpu);
    if (is_file_out) fclose(fout);
    if (do_detail) printf("[Done]");

    return EXIT_SUCCESS;
}