#include <stdio.h>

#include "_include.c"

int main() {
    struct CPU _cpu = cpu_create(0xFFF);
    struct CPU *cpu = &_cpu;

    cpu->regs[REG_SSIZE] = 0x1FF;

    int addr = 0;

    // CODE
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}