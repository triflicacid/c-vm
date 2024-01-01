#include <stdio.h>

#include "cpu.h"
#include "fetch-exec.h"

int main() {
    struct CPU _cpu = cpu_create(0xFFF);
    struct CPU *cpu = &_cpu;

    cpu->regs[REG_SSIZE] = 0x1FF;
    for (int i = 0; i < REG_RESV; ++i) cpu->regs[i] = i;

    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_CALL_LIT);
    MEM_WRITE(addr, UWORD_T, 256);
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);
    addr = 256;
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_CALL_LIT);
    MEM_WRITE(addr, UWORD_T, 512);
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_RET);
    addr = 512;
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_RET);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}