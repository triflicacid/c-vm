#include <stdio.h>

#include "../../src/cpu/cpu.c"
#include "../../src/cpu/err.c"
#include "../../src/cpu/fetch-exec.c"
#include "../../src/cpu/opcodes.h"
#include "../../src/util.c"

int main() {
    struct CPU _cpu = cpu_create(8, 512);
    struct CPU *cpu = &_cpu;

    // Expected
    T_f64 a = 3.14, b = -a;
    T_i64 ai = *(T_i64 *)&a, bi = *(T_i64 *)&b;
    printf("-(%lf) = %lf\n", a, b, ai, bi);
    printf("a: ");
    print_bytes(&a, sizeof(T_f64));
    printf("\nb: ");
    print_bytes(&b, sizeof(T_f64));
    printf("\n");

    // Load program
    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_MOV64_LIT_REG);
    MEM_WRITE(addr, T_i64, ai);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_NEGD);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}