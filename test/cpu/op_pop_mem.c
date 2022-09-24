#include <stdio.h>

#include "_include.c"

int main() {
    struct CPU _cpu = cpu_create(0xFFF);
    struct CPU *cpu = &_cpu;

    const T_u8 bytes[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    const int bytes_addr = 256;
    cpu->regs[REG_SSIZE] = sizeof(bytes);

    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_PUSHN_LIT);
    MEM_WRITE(addr, T_u8, sizeof(bytes));
    MEM_WRITE_BYTES(addr, bytes, sizeof(bytes));
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_POPN_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes) / 2);
    MEM_WRITE(addr, UWORD_T, bytes_addr);
    MEM_WRITE(addr, OPCODE_T, OP_PSTACK);
    MEM_WRITE(addr, OPCODE_T, OP_PRINT_HEX_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes));
    MEM_WRITE(addr, UWORD_T, bytes_addr);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}