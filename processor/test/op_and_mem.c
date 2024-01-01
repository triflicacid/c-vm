#include <stdio.h>

#include "cpu.h"
#include "fetch-exec.h"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    const T_u8 bytes1[] = {0x01, 0x02, 0x03, 0x04, 0x05,
                           0x06, 0x07, 0x08, 0x09, 0x0A};
    const T_u8 bytes2[] = {0x10, 0x20, 0x30, 0x40, 0x50,
                           0x60, 0x70, 0x80, 0x90, 0xA0};
    const UWORD_T bytes_addr = 256;

    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_MOVN_LIT_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes1));
    MEM_WRITE(addr, UWORD_T, bytes_addr);
    MEM_WRITE_BYTES(addr, bytes1, sizeof(bytes1));
    MEM_WRITE(addr, OPCODE_T, OP_MOVN_LIT_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes2));
    MEM_WRITE(addr, UWORD_T, bytes_addr + sizeof(bytes1));
    MEM_WRITE_BYTES(addr, bytes2, sizeof(bytes2));
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);
    cpu_mem_print(cpu, bytes_addr, sizeof(bytes1) + sizeof(bytes2), 1,
                  sizeof(bytes1));

    MEM_WRITE(addr, OPCODE_T, OP_AND_MEM_MEM);
    MEM_WRITE(addr, T_u8, sizeof(bytes1));
    MEM_WRITE(addr, UWORD_T, bytes_addr);
    MEM_WRITE(addr, UWORD_T, bytes_addr + sizeof(bytes1));
    MEM_WRITE(addr, OPCODE_T, OP_HALT);

    cpu_fecycle(cpu);
    cpu_mem_print(cpu, bytes_addr, sizeof(bytes1) + sizeof(bytes2), 1,
                  sizeof(bytes1));

    cpu_destroy(cpu);
    printf("Done.");
}