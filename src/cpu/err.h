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

// Unknown register offset
#define ERR_REG 2

// Unknown instruction. Instruction = .err_data
#define ERR_UNINST 3

/** Print error information */
void err_print(struct CPU *cpu);

#endif