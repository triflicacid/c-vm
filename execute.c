#include <stdio.h>

#include "src/cpu/bit-ops.c"
#include "src/cpu/cpu.c"
#include "src/cpu/err.c"
#include "src/cpu/fetch-exec.c"
#include "src/cpu/opcodes.h"
#include "src/util.c"

int main(int argc, char **argv) {
    char *file_in;
    int is_file_in = 0, do_detail = 0;
    UWORD_T addr_start = 0, mem_size = 0xFFF, stack_size = 0x1FF;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'p':  // Print detail
                    do_detail = !do_detail;
                    break;
                case 'm':  // Memory size
                    i++;
                    if (i >= argc) {
                        printf("-m: expected number\n");
                        return -1;
                    }
                    mem_size = strtoull(
                        argv[i], (char **)(argv[i] + strlen(argv[0])), 10);
                    break;
                case 's':  // Stack size
                    i++;
                    if (i >= argc) {
                        printf("-s: expected number\n");
                        return -1;
                    }
                    stack_size = strtoull(
                        argv[i], (char **)(argv[i] + strlen(argv[0])), 10);
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

    struct CPU _cpu = cpu_create(mem_size);
    struct CPU *cpu = &_cpu;
    cpu->regs[REG_SSIZE] = stack_size;

    if (do_detail) {
        printf("Initlialising CPU...\n");
        cpu_print_details(cpu);
        printf("\n");
    }

    FILE *fp = fopen((is_file_in ? file_in : "source.bin"), "r");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);
    if (do_detail)
        printf("Reading source file '%s' (%li bytes)\n\n",
               (is_file_in ? file_in : "source.bin"), fsize);
    cpu_mem_fread(cpu, fp, addr_start, fsize);
    fclose(fp);

    // if (do_detail) {
    //     if (o.errc != ASM_ERR_NONE) printf("\n");
    //     printf("Buffer Size : %llu\n", buf_size);
    //     printf("Buffer Used : %u\n", o.buf_offset);
    //     printf("Line        : %u\n", o.line);
    //     printf("Col         : %u\n", o.col);
    //     printf("Errno       : %i\n", o.errc);
    // } else {
    //     printf("%i", o.errc);
    // }

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    if (do_detail) printf("[Done]");

    return 0;
}