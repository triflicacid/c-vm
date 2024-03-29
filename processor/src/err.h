#ifndef CPU_ERR_H_
#define CPU_ERR_H_

#define ERRNO_T int

#include "cpu.h"

// CPU - set error. Requires `CPU cpu`
#define ERR_SET(errn, data)         \
    {                               \
        cpu->regs[REG_ERR] = errn;  \
        cpu->regs[REG_FLAG] = data; \
    }

// No error
#define ERR_NONE 0

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

// Stack underflow
#define ERR_STACK_UFLOW 4

// Stack overflow
#define ERR_STACK_OFLOW 5

// Check for stack overflow
#define ERR_CHECK_STACK_OFLOW()                                         \
    if (cpu->regs[REG_SP] < cpu->mem_size - 1 - cpu->regs[REG_STACK_SIZE]) { \
        ERR_SET(ERR_STACK_OFLOW, cpu->regs[REG_SP]);                    \
    }

// Check for stack underflow
#define ERR_CHECK_STACK_UFLOW()              \
    if (cpu->regs[REG_SP] > cpu->mem_size) { \
        ERR_SET(ERR_STACK_UFLOW, 0);         \
    }

// Unknown syscall operation
#define ERR_SYSCAL 6

#endif