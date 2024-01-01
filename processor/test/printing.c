#include <stdio.h>

#include "cpu.h"
#include "fetch-exec.h"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    const char string[] = "Hello, World!\n";

    int addr = 0;

    MEM_WRITE(addr, OPCODE_T, OP_MOVN_LIT_MEM);
    MEM_WRITE(addr, T_u8, sizeof(string) - 1);
    MEM_WRITE(addr, UWORD_T, 256);
    MEM_WRITE_BYTES(addr, string, sizeof(string) - 1);
    MEM_WRITE(addr, OPCODE_T, OP_PRINT_CHARS_MEM);
    MEM_WRITE(addr, T_u8, sizeof(string) - 1);
    MEM_WRITE(addr, UWORD_T, 256);
    MEM_WRITE(addr, OPCODE_T, OP_MOV_LIT_REG);
    MEM_WRITE(addr, WORD_T, 0x0a3d4e454255523d);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_PRINT_HEX_REG);
    // MEM_WRITE(addr, OPCODE_T, OP_PRINT_CHARS_REG);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}