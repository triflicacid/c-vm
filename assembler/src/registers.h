#ifndef ASM_REGISTERS_H_
#define ASM_REGISTERS_H_

#include "processor/src/registers.h"
#include "util.h"

/** Given a register mnemonic, return its offset, or -1. */
T_i8 cpu_reg_offset_from_string(const char *string);

#endif
