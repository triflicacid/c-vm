#include <stdio.h>

#include "_include.c"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    const T_u8 bytes[] = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                          0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF};
    const UWORD_T bytes_addr = 256;

    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_MOVN_LIT_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes));
    MEM_WRITE(addr, UWORD_T, bytes_addr);
    for (T_u8 i = 0; i < sizeof(bytes); ++i) MEM_WRITE(addr, T_u8, bytes[i]);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);
    cpu_mem_print(cpu, bytes_addr, sizeof(bytes), 1, 12);

    MEM_WRITE(addr, OPCODE_T, OP_NOT_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes));
    MEM_WRITE(addr, UWORD_T, bytes_addr);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);
    cpu_mem_print(cpu, bytes_addr, sizeof(bytes), 1, 12);

    cpu_destroy(cpu);
    printf("Done.");
}