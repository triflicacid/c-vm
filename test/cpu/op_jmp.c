#include <stdio.h>

#include "_include.c"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    int addr = 0, addr2 = 256;
    cpu->regs[1] = addr2;

    MEM_WRITE(addr, OPCODE_T, OP_JMP_REG);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_MOV_LIT_REG);
    MEM_WRITE(addr, WORD_T, 1);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    MEM_WRITE(addr2, OPCODE_T, OP_MOV_LIT_REG);
    MEM_WRITE(addr2, WORD_T, -1);
    MEM_WRITE(addr2, T_u8, 1);
    MEM_WRITE(addr2, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_reg_print(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}