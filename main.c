#include <stdio.h>

#include "src/cpu/bit-ops.c"
#include "src/cpu/cpu.c"
#include "src/cpu/err.c"
#include "src/cpu/fetch-exec.c"
#include "src/cpu/opcodes.h"
#include "src/util.c"

int main() {
    struct CPU _cpu = cpu_create(0xFFF);
    struct CPU *cpu = &_cpu;

    cpu->regs[REG_SSIZE] = 0x1FF;

    int addr = 0;

    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}