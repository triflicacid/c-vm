#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "bit-ops.h"
#include "cpu.h"
#include "fetch-exec.h"

int main(int argc, char **argv) {
    char *file_in, *file_out;
    bool is_file_in = 0, is_file_out = 0, do_detail = 0;
    UWORD_T addr_start = 0, mem_size = 0xFFF, stack_size = 0x1FF;
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
                    mem_size = strtoull(
                        argv[i], (char **)(argv[i] + strlen(argv[0])), 10);
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

                    stack_size = strtoull(
                        argv[i], (char **)(argv[i] + strlen(argv[0])), 10);
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

    struct CPU _cpu = cpu_create(mem_size);
    struct CPU *cpu = &_cpu;
    cpu->regs[REG_SSIZE] = stack_size;

    FILE *fout;
    if (is_file_out) {
        if (do_detail) printf("Set STDOUT='%s'\n", file_out);
        fout = fopen(file_out, "w");
        cpu->out = fout;
    }

    if (do_detail) {
        printf("Initlialising CPU...\n");
        cpu_print_details(cpu);
        printf("\n");
    }

    FILE *fp = fopen((is_file_in ? file_in : "source.bin"), "rb");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);
    if (do_detail)
        printf("Reading source file '%s' (%li bytes)... ",
               (is_file_in ? file_in : "source.bin"), fsize);
    cpu_mem_fread(cpu, fp, addr_start, fsize);
    fclose(fp);
    if (do_detail)
        printf("Done.\n\n");

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    if (is_file_out) fclose(fout);
    if (do_detail) printf("[Done]");

    return EXIT_SUCCESS;
}