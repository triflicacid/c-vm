#include <stdio.h>

#include "src/assembler/assemble.c"
#include "src/cpu/bit-ops.c"
#include "src/cpu/cpu.c"
#include "src/cpu/err.c"
#include "src/cpu/fetch-exec.c"
#include "src/cpu/opcodes.h"
#include "src/util.c"

int main(int argc, char **argv) {
    struct CPU _cpu = cpu_create(0xFFF);
    struct CPU *cpu = &_cpu;
    cpu->regs[REG_SSIZE] = 0x1FF;

    FILE *fp =
        fopen((argc > 1 && strlen(argv[1]) > 0) ? argv[1] : "source.asm", "r");
    struct Assemble o =
        assemble(fp, cpu->mem, cpu->mem_size - cpu->regs[REG_SSIZE] * 2);
    fclose(fp);

    printf("-----\n");
    printf("Buffer Offset: %i\n", o.buf_offset);
    printf("Line         : %i\n", o.line);
    printf("Col          : %i\n", o.col);
    printf("Errno        : %i\n", o.errc);

    cpu_destroy(cpu);
    printf("Done.");
}