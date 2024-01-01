#include <stdio.h>

#include "cpu.h"
#include "fetch-exec.h"

int main() {
    struct CPU _cpu = cpu_create(0xFFF);
    struct CPU *cpu = &_cpu;

    cpu->regs[REG_SSIZE] = 8;
    cpu->regs[1] = 0xABCDEF;

    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_PUSH32_LIT);
    MEM_WRITE(addr, T_u32, -1);
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}