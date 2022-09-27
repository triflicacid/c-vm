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
    T_u8 buffer[BUF_SIZE];

    FILE *fp =
        fopen((argc > 1 && strlen(argv[1]) > 0) ? argv[1] : "source.asm", "r");
    struct Assemble o = assemble(fp, buffer, BUF_SIZE);
    fclose(fp);

    if (o.errc != ASM_ERR_NONE) printf("\n");
    printf("Buffer Offset: %u\n", o.buf_offset);
    printf("Line         : %u\n", o.line);
    printf("Col          : %u\n", o.col);
    printf("Errno        : %i\n", o.errc);

    if (argc > 2) {
        fp = fopen(argv[2], "w");
        fwrite(buffer, o.buf_offset, 1, fp);
        fclose(fp);
        printf("\nWritten %u bytes to file '%s'\n", o.buf_offset, argv[2]);
    }
}