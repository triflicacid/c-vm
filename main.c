#include <stdio.h>

#include "cpu/cpu.c"
#include "cpu/err.c"
#include "cpu/fetch-exec.c"
#include "cpu/opcodes.h"

void print_bytes(const void *data, const unsigned int len) {
    for (unsigned int off = 0; off < len; ++off) {
        printf("%.2X ", *((unsigned char *)data + off));
    }
    printf("\n");
}

int main() {
    struct CPU _cpu = cpu_create(8, 512);
    struct CPU *cpu = &_cpu;

    // T_f64 a = 5.9, b = 16.714, ab = a + b;
    // T_i64 ai = *(T_i64 *)&a, bi = *(T_i64 *)&b, abi = *(T_i64 *)&ab;

    // printf("%lf + %lf = %lf\n%X + %X = %X\n\n", a, b, ab, ai, bi, abi);

    // int addr = 0;
    // MEM_WRITE(addr, OPCODE_T, OP_MOV64_LIT_REG);
    // MEM_WRITE(addr, T_f64, a);
    // MEM_WRITE(addr, T_u8, 1);
    // MEM_WRITE(addr, OPCODE_T, OP_MOV64_LIT_REG);
    // MEM_WRITE(addr, T_f64, b);
    // MEM_WRITE(addr, T_u8, 2);
    // MEM_WRITE(addr, OPCODE_T, OP_PREG);
    // MEM_WRITE(addr, OPCODE_T, OP_ADDF64_REG_REG);
    // MEM_WRITE(addr, T_u8, 1);
    // MEM_WRITE(addr, T_u8, 2);
    // MEM_WRITE(addr, OPCODE_T, OP_PREG);
    // MEM_WRITE(addr, OPCODE_T, OP_HALT);

    T_f64 a = 3.14, b = -a;
    T_i64 ai = *(T_i64 *)&a, bi = *(T_i64 *)&b;
    printf("-(%lf) = %lf\n-(%X) = %X\n", a, b, ai, bi);
    print_bytes(&a, 8);
    print_bytes(&b, 8);

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