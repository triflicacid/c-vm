#include <stdio.h>

#include "cpu.h"
#include "fetch-exec.h"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    T_f64 a = 5.9, b = 16.714, ab = a + b;
    T_i64 ai = *(T_i64 *)&a, bi = *(T_i64 *)&b, abi = *(T_i64 *)&ab;

    printf("%lf + %lf = %lf\n", a, b, ab);
    print_bytes(&ai, sizeof(T_i64));
    printf("+\n");
    print_bytes(&bi, sizeof(T_i64));
    printf("=\n");
    print_bytes(&abi, sizeof(T_i64));
    printf("\n\n");

    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_MOV64_LIT_REG);
    MEM_WRITE(addr, T_f64, a);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_MOV64_LIT_REG);
    MEM_WRITE(addr, T_f64, b);
    MEM_WRITE(addr, T_u8, 2);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_ADDF64_REG_REG);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, T_u8, 2);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}