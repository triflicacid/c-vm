#include <stdio.h>

#include "_include.c"

#define PRINT_REG(reg)                             \
    MEM_WRITE(addr, OPCODE_T, OP_PRINT_HEX_REG);   \
    MEM_WRITE(addr, T_u8, reg);                    \
    MEM_WRITE(addr, OPCODE_T, OP_PRINT_CHARS_LIT); \
    MEM_WRITE(addr, T_u8, reg);                    \
    MEM_WRITE(addr, T_u8, '\n');

int main() {
    struct CPU _cpu = cpu_create(0xFFF);
    struct CPU *cpu = &_cpu;

    cpu->regs[REG_SSIZE] = 0x1FF;

    int addr = 0, reg = 1;

    PRINT_REG(reg)
    MEM_WRITE(addr, OPCODE_T, OP_GET_CHAR);
    MEM_WRITE(addr, T_u8, reg);
    PRINT_REG(reg)
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}