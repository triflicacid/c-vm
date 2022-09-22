#include <stdio.h>

#include "_include.c"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    const T_u8 bytes1[] = {0x03, 0x00};
    const T_u8 bytes2[] = {0x02, 0x00};
    const UWORD_T byte_addr = 256;

    int addr = 0;

    MEM_WRITE(addr, OPCODE_T, OP_MOVN_LIT_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes1));
    MEM_WRITE(addr, UWORD_T, byte_addr);
    MEM_WRITE_BYTES(addr, bytes1, sizeof(bytes1));
    MEM_WRITE(addr, OPCODE_T, OP_MOVN_LIT_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes2));
    MEM_WRITE(addr, UWORD_T, byte_addr + sizeof(bytes1));
    MEM_WRITE_BYTES(addr, bytes2, sizeof(bytes2));
    MEM_WRITE(addr, OPCODE_T, OP_CMP_MEM_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes1));
    MEM_WRITE(addr, UWORD_T, byte_addr);
    MEM_WRITE(addr, UWORD_T, byte_addr + sizeof(bytes1));
    MEM_WRITE(addr, OPCODE_T, OP_PREG);
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}