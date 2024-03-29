#ifndef CPU_REGISTERS_H_
#define CPU_REGISTERS_H_

// Registers: general 0-9, ip
// Register for comparison operations
#define REG_CMP 9
#define REG_CMP_SYM "cp"
// Register for flags
#define REG_FLAG 9
#define REG_FLAG_SYM "fl"
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
#define REG_STACK_SIZE 14
#define REG_STACK_SIZE_SYM "ss"
// Frame pointer
#define REG_FP 15
#define REG_FP_SYM "fp"
// Total number of registers
#define REG_COUNT 16
// Preserve first `n` registers
#define REG_RESV 5

#include "util.h"

// Size of preserved data section in stack frame
#define STACK_FRAME_PRESERVED_DATA_SIZE (sizeof(WORD_T) * REG_RESV + sizeof(UWORD_T) * 2)

// == CCR Bits ==
//#define CCR_CARRY 0
//#define CCR_OVERFL 1
//#define CCR_ZERO 2
//#define CCR_MINUS 3
//
//// Set states of the CCR - pass 0 or 1 to update, else pass -1. Must have `CPU cpu`
//#define SET_CCR(C, V, Z, N) \
//    if (C > -1) cpu->regs[REG_CCR] = (cpu->regs[REG_CCR] & ~1) | C;\
//    if (V > -1) cpu->regs[REG_CCR] = (cpu->regs[REG_CCR] & ~2) | (V << 1);\
//    if (Z > -1) cpu->regs[REG_CCR] = (cpu->regs[REG_CCR] & ~4) | (Z << 2);\
//    if (N > -1) cpu->regs[REG_CCR] = (cpu->regs[REG_CCR] & ~8) | (N << 3);

#endif