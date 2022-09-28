#include <stdio.h>

#include "src/assembler/assemble.c"
#include "src/cpu/bit-ops.c"
#include "src/cpu/cpu.c"
#include "src/cpu/err.c"
#include "src/cpu/fetch-exec.c"
#include "src/cpu/opcodes.h"
#include "src/util.c"

#define BUF_SIZE 0x1FF

int main(int argc, char **argv) {
    T_u8 buffer[0x1FF];

    char *file_in, *file_out;
    int is_file_in = 0, is_file_out = 0, do_detail = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'o':
                    is_file_out = 1;
                    i++;
                    file_out = argv[i];
                    break;
                case 'p':
                    do_detail = !do_detail;
                    break;
                default:
                    printf("Unknown option '%s'\n", argv[i]);
                    return 1;
            }
        } else if (!is_file_in) {
            is_file_in = 1;
            file_in = argv[i];
        } else {
            printf("Unknown argument '%s'\n", argv[i]);
            return 1;
        }
    }

    if (do_detail)
        printf("Reading source file '%s'\n\n",
               is_file_in ? file_in : "source.asm");
    FILE *fp = fopen(is_file_in ? file_in : "source.asm", "r");
    struct Assemble o = assemble(fp, buffer, sizeof(buffer), do_detail);
    fclose(fp);

    if (do_detail) {
        if (o.errc != ASM_ERR_NONE) printf("\n");
        printf("Buffer Offset: %u\n", o.buf_offset);
        printf("Line         : %u\n", o.line);
        printf("Col          : %u\n", o.col);
        printf("Errno        : %i\n", o.errc);
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

    return o.errc;
}