#ifndef __REGISTERS_H__
#define __REGISTERS_H__

// Registers: general 0-9, ip
// Register for flags
#define REG_FLAG 9
#define REG_FLAG_SYM "fl"
// Register for comparison operations
#define REG_CMP 9
#define REG_CMP_SYM "cp"
// Error code
#define REG_ERR 10
#define REG_ERR_SYM "er"
// Condition Codes
#define REG_CCR 11
#define REG_CCR_SYM "ccr"
// Instruction pointer
#define REG_IP 12
#define REG_IP_SYM "ip"
// Stack pointer
#define REG_SP 13
#define REG_SP_SYM "sp"
// Stores size (bytes) of stack
#define REG_SSIZE 14
#define REG_SSIZE_SYM "ss"
// Frame pointer
#define REG_FP 15
#define REG_FP_SYM "fp"
// Total number of registers
#define REG_COUNT 16
// Preserve first `n` registers
#define REG_RESV 5

// == CCR Bits ==
#define CCR_CARRY 0
#define CCR_OVERFL 1
#define CCR_ZERO 2
#define CCR_MINUS 3

// Set states of the CCR - pass 0 or 1 to update, else pass -1. Must have `struct CPU *cpu`
#define SET_CCR(C, V, Z, N) \
    if (C > -1) cpu->regs[REG_CCR] = (cpu->regs[REG_CCR] & ~1) | C;\
    if (V > -1) cpu->regs[REG_CCR] = (cpu->regs[REG_CCR] & ~2) | (V << 1);\
    if (Z > -1) cpu->regs[REG_CCR] = (cpu->regs[REG_CCR] & ~4) | (Z << 2);\
    if (N > -1) cpu->regs[REG_CCR] = (cpu->regs[REG_CCR] & ~8) | (N << 3);

#include "cpu.h"
#include "err.h"

/** Given a register mnemonic, return its offset, or -1. */
T_i8 cpu_reg_offset_from_string(const char *string);

/** Print register contents */
int cpu_reg_print(struct CPU *cpu);

/** Set contents of register */
void cpu_reg_write(struct CPU *cpu, unsigned int reg_offset, WORD_T value);

/** Get contents of register Instruction Pointer */
WORD_T cpu_reg_read(struct CPU *cpu, unsigned int reg_offset);

#endif