#include <stdio.h>

#include "../../src/cpu/cpu.c"
#include "../../src/cpu/err.c"
#include "../../src/cpu/fetch-exec.c"
#include "../../src/cpu/opcodes.h"
#include "../../src/util.c"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    // Expected
    WORD_T a = 657, na = -a;
    printf("-(%lli) = %lli\n", a, na);
    printf("a: ");
    print_bytes(&a, sizeof(WORD_T));
    printf("\n-a: ");
    print_bytes(&na, sizeof(WORD_T));
    printf("\n");

    // Load program
    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_MOV_LIT_REG);
    MEM_WRITE(addr, WORD_T, a);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_NEG);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}