#ifndef __ERR_H__
#define __ERR_H__
#include "cpu.h"

// CPU - set error. Requires `struct CPU *cpu`
#define ERR_SET(errn, data)   \
    {                         \
        cpu->err = errn;      \
        cpu->err_data = data; \
    }

// No error
#define ERR_NONE 0

// Set CPUs error as ERR_NONE. Requires `struct CPU *cpu`.
#define ERR_CLEAR() (cpu->err = ERR_NONE)

// Memory OutOfBounds. Address = CPU.err_data
#define ERR_MEMOOB 1

// MACRO - check for Memory OOB
#define ERR_CHECK_ADDR(addr)       \
    if (addr >= cpu->mem_size) {   \
        ERR_SET(ERR_MEMOOB, addr); \
    }

// Unknown register offset
#define ERR_REG 2

// MACRO - check for invalid register
#define ERR_CHECK_REG(reg)     \
    if (reg >= REG_COUNT) {    \
        ERR_SET(ERR_REG, reg); \
    }

// Unknown instruction. Instruction = .err_data
#define ERR_UNINST 3

/** Print error information */
void err_print(struct CPU *cpu);

#endif