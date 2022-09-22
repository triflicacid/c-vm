#include <stdio.h>

#include "_include.c"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    WORD_T a = 666, b = 15, c = a / b, d = a % b;
    printf("%lli / %lli = %lli r %lli\n", a, b, c, d);

    printf("  ");
    print_bytes(&a, sizeof(WORD_T));
    printf("\n/ ");
    print_bytes(&b, sizeof(WORD_T));
    printf("\n= ");
    print_bytes(&c, sizeof(WORD_T));
    printf("\nr ");
    print_bytes(&d, sizeof(WORD_T));
    printf("\n\n");

    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_MOV_LIT_REG);
    MEM_WRITE(addr, WORD_T, a);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_MOV_LIT_REG);
    MEM_WRITE(addr, WORD_T, b);
    MEM_WRITE(addr, T_u8, 2);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_DIV_REG_REG);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, T_u8, 2);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}