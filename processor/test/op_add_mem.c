#include <stdio.h>

#include "cpu.h"
#include "fetch-exec.h"

int main() {
    struct CPU _cpu = cpu_create(512);
    struct CPU *cpu = &_cpu;

    const T_u8 bytes1[] = {0xFF, 0xFF};
    const T_u8 bytes2[] = {0xFF, 0xFF};
    // 0xFF + 0xFF = 0x1FFFE so expect 0xFFFE overflow 1
    const UWORD_T bytes_addr = 256;

    int addr = 0;
    MEM_WRITE(addr, OPCODE_T, OP_MOVN_LIT_MEM)
    MEM_WRITE(addr, T_u8, sizeof(bytes1))
    MEM_WRITE(addr, UWORD_T, bytes_addr)
    MEM_WRITE_BYTES(addr, bytes1, sizeof(bytes1))
    MEM_WRITE(addr, OPCODE_T, OP_MOVN_LIT_MEM)
    MEM_WRITE(addr, T_u8, sizeof(bytes2))
    MEM_WRITE(addr, UWORD_T, bytes_addr + sizeof(bytes1))
    MEM_WRITE_BYTES(addr, bytes2, sizeof(bytes2))
    MEM_WRITE(addr, OPCODE_T, OP_HALT)

    cpu_fecycle(cpu);
    cpu_mem_print(cpu, bytes_addr, sizeof(bytes1) + sizeof(bytes2), 1,
                  sizeof(bytes1));

    MEM_WRITE(addr, OPCODE_T, OP_ADD_MEM_MEM)
    MEM_WRITE(addr, T_u8, sizeof(bytes1))
    MEM_WRITE(addr, UWORD_T, bytes_addr)
    MEM_WRITE(addr, UWORD_T, bytes_addr + sizeof(bytes1))
    MEM_WRITE(addr, OPCODE_T, OP_HALT)

    cpu_fecycle(cpu);
    cpu_mem_print(cpu, bytes_addr, sizeof(bytes1) + sizeof(bytes2), 1,
                  sizeof(bytes1));
    cpu_reg_print(cpu);

    cpu_destroy(cpu);
    printf("Done.");
}