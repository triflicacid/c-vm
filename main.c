#include <stdio.h>

#include "src/cpu/bit-ops.c"
#include "src/cpu/cpu.c"
#include "src/cpu/err.c"
#include "src/cpu/fetch-exec.c"
#include "src/cpu/opcodes.h"
#include "src/util.c"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    const T_u8 bytes[] = "Let's trigger an error!\n";
    int addr = 0;

    MEM_WRITE(addr, OPCODE_T, OP_PRINT_CHARS_LIT);
    MEM_WRITE(addr, T_u8, sizeof(bytes) - 1);
    MEM_WRITE_BYTES(addr, bytes, sizeof(bytes) - 1);
    MEM_WRITE(addr, OPCODE_T, OP_MOV_LIT_REG);
    MEM_WRITE(addr, WORD_T, 0xDEAD);
    MEM_WRITE(addr, T_u8, REG_ERR);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_reg_print(cpu);
    cpu_destroy(cpu);
    printf("Done.");
}