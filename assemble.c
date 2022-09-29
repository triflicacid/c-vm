#include "src/assembler/assemble.c"

#include <stdio.h>

#include "src/cpu/cpu.c"
#include "src/cpu/opcodes.h"
#include "src/util.c"

int main(int argc, char **argv) {
    char *file_in, *file_out;
    int is_file_in = 0, is_file_out = 0, do_detail = 0;
    unsigned long long buf_size = 1000000;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'o':  // Out file
                    i++;
                    if (i >= argc) {
                        printf("-o: expected file path\n");
                        return -1;
                    }
                    is_file_out = 1;
                    file_out = argv[i];
                    break;
                case 'b':  // Set buffer size
                    i++;
                    if (i >= argc) {
                        printf("-b: expected number\n");
                        return -1;
                    }
                    buf_size = strtoull(
                        argv[i], (char **)(argv[i] + strlen(argv[0])), 10);
                    break;
                case 'p':  // Print detail
                    do_detail = !do_detail;
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

    char *buffer = (char *)malloc(buf_size);

    if (do_detail)
        printf("Reading source file '%s'\n\n",
               is_file_in ? file_in : "source.asm");
    FILE *fp = fopen(is_file_in ? file_in : "source.asm", "r");
    struct Assemble o = assemble(fp, buffer, buf_size, do_detail);
    fclose(fp);

    if (do_detail) {
        if (o.errc != ASM_ERR_NONE) printf("\n");
        printf("Buffer Size : %llu\n", buf_size);
        printf("Buffer Used : %u\n", o.buf_offset);
        printf("Line        : %u\n", o.line);
        printf("Col         : %u\n", o.col);
        printf("Errno       : %i\n", o.errc);
    } else {
        printf("%i", o.errc);
    }

    if (o.buf_offset != 0) {
        fp = fopen(is_file_out ? file_out : "source.bin", "w");
        fwrite(buffer, o.buf_offset, 1, fp);
        fclose(fp);
        if (do_detail)
            printf("\nWritten %u bytes to file '%s'\n", o.buf_offset,
                   is_file_out ? file_out : "source.bin");
    }

    free(buffer);

    return o.errc;
}