#include <stdio.h>

#include "cpu.h"
#include "fetch-exec.h"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    int addr = 0;

    MEM_WRITE(addr, OPCODE_T, OP_MOV64_LIT_REG);
    MEM_WRITE(addr, T_f64, 314.15);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, OPCODE_T, OP_MOV64_LIT_REG);
    MEM_WRITE(addr, T_f64, 2.712);
    MEM_WRITE(addr, T_u8, 2);
    MEM_WRITE(addr, OPCODE_T, OP_CMPF64_REG_REG);
    MEM_WRITE(addr, T_u8, 1);
    MEM_WRITE(addr, T_u8, 2);
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}