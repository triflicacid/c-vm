#include <stdio.h>

#include "_include.c"

int main() {
    struct CPU _cpu = cpu_create(0xFFF);
    struct CPU *cpu = &_cpu;

    cpu->regs[REG_SSIZE] = 0x1FF;

    int addr = 0, addr2 = 512;

    MEM_WRITE(addr, OPCODE_T, OP_PRINT_CHARS_LIT);
    const T_u8 str1[] = "Outside 1\n";
    MEM_WRITE(addr, T_u8, sizeof(str1) - 1);
    MEM_WRITE_BYTES(addr, str1, sizeof(str1) - 1);
    MEM_WRITE(addr, OPCODE_T, OP_CALL_LIT);
    MEM_WRITE(addr, UWORD_T, addr2);
    MEM_WRITE(addr, OPCODE_T, OP_PRINT_CHARS_LIT);
    const T_u8 str2[] = "Outside 2\n";
    MEM_WRITE(addr, T_u8, sizeof(str2) - 1);
    MEM_WRITE_BYTES(addr, str2, sizeof(str2) - 1);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    MEM_WRITE(addr2, OPCODE_T, OP_PRINT_CHARS_LIT);
    const T_u8 str3[] = "Inside 1\n";
    MEM_WRITE(addr2, T_u8, sizeof(str3) - 1);
    MEM_WRITE_BYTES(addr2, str3, sizeof(str3) - 1);
    MEM_WRITE(addr2, OPCODE_T, OP_RET);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}