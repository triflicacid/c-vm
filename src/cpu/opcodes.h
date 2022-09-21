#ifndef __OPCODES_H__
#define __OPCODES_H__

#include "cpu.h"

#define OPCODE_T T_u16
#define OPCODE_T_FLAG "%u"

// No-operation, skip
#define OP_NOP 0x0000
// HALT
#define OP_HALT 0xFFFF
// DEBUG: PRINT MEMORY
#define OP_PMEM 0xFFFA
// DEBUG: PRINT REGISTERS
#define OP_PREG 0xFFFB

// Move literal to register.
// Syntax: `mov <data: word> <register: u8>`
#define OP_MOV_LIT_REG 0x0010
// Move 8-bit literal to register.
// Syntax: `mov <data: 1 byte> <register: u8>`
#define OP_MOV8_LIT_REG 0x0011
// Move 16-bit literal to register.
// Syntax: `mov <data: 2 bytes> <register: u8>`
#define OP_MOV16_LIT_REG 0x0012
// Move 32-bit literal to register.
// Syntax: `mov <data: 4 bytes> <register: u8>`
#define OP_MOV32_LIT_REG 0x0013
// Move 64-bit literal to register.
// Syntax: `mov <data: 8 bytes> <register: u8>`
#define OP_MOV64_LIT_REG 0x0014

// Move literal to memory address.
// Syntax: `mov <data: word> <address: unsigned word>`
#define OP_MOV_LIT_MEM 0x0015
// Move 8-bit literal to memory address.
// Syntax: `mov <data: 1 byte> <address: unsigned word>`
#define OP_MOV8_LIT_MEM 0x0016
// Move 16-bit literal to memory address.
// Syntax: `mov <data: 2 bytes> <address: unsigned word>`
#define OP_MOV16_LIT_MEM 0x0017
// Move 32-bit literal to memory address.
// Syntax: `mov <data: 4 bytes> <address: unsigned word>`
#define OP_MOV32_LIT_MEM 0x0018
// Move 64-bit literal to memory address.
// Syntax: `mov <data: 8 bytes> <address: unsigned word>`
#define OP_MOV64_LIT_MEM 0x0019
// Move k-bit literal to memory address.
// Syntax: `mov <bytes: u8> <data: n bytes> <address: unsigned word>`
#define OP_MOVN_LIT_MEM 0x001A

// Move value stored at address [register + memory address], and store in
// register. Syntax: `mov <addr: unsigned word> <register: u8> <register: u8>`
#define OP_MOV_LIT_OFF_REG 0x001B
// Move 8-bit value stored at address [register + memory address], and store in
// register. Syntax: `mov <addr: unsigned word> <register: u8> <register: u8>`
#define OP_MOV8_LIT_OFF_REG 0x001C
// Move 16-bit value stored at address [register + memory address], and store in
// register. Syntax: `mov <addr: unsigned word> <register: u8> <register: u8>`
#define OP_MOV16_LIT_OFF_REG 0x001D
// Move 32-bit value stored at address [register + memory address], and store in
// register. Syntax: `mov <addr: unsigned word> <register: u8> <register: u8>`
#define OP_MOV32_LIT_OFF_REG 0x001E
// Move 64-bit value stored at address [register + memory address], and store in
// register. Syntax: `mov <addr: unsigned word> <register: u8> <register: u8>`
#define OP_MOV64_LIT_OFF_REG 0x001F

// Move word value at address to given register
// Syntax: `mov <addr: unsigned word> <register: u8>`
#define OP_MOV_MEM_REG 0x0020
// Move 8-bit value at address to given register
// Syntax: `mov <addr: unsigned word> <register: u8>`
#define OP_MOV8_MEM_REG 0x0021
// Move 16-bit value at address to given register
// Syntax: `mov <addr: unsigned word> <register: u8>`
#define OP_MOV16_MEM_REG 0x0022
// Move 32-bit value at address to given register
// Syntax: `mov <addr: unsigned word> <register: u8>`
#define OP_MOV32_MEM_REG 0x0023
// Move 64-bit value at address to given register
// Syntax: `mov <addr: unsigned word> <register: u8>`
#define OP_MOV64_MEM_REG 0x0024

// Move register contents to memory address
// Syntax: `mov <register: u8> <addr: unsigned word>`
#define OP_MOV_REG_MEM 0x002A
// Move register contents to memory address as 8-bit word
// Syntax: `mov <register: u8> <addr: unsigned word>`
#define OP_MOV8_REG_MEM 0x002B
// Move register contents to memory address as 16-bit word
// Syntax: `mov <register: u8> <addr: unsigned word>`
#define OP_MOV16_REG_MEM 0x002C
// Move register contents to memory address as 32-bit word
// Syntax: `mov <register: u8> <addr: unsigned word>`
#define OP_MOV32_REG_MEM 0x002D
// Move register contents to memory address as 64-bit word
// Syntax: `mov <register: u8> <addr: unsigned word>`
#define OP_MOV64_REG_MEM 0x002E

// Move value in memory whose address is stored in a register to another
// register. Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV_REGPTR_REG 0x0030
// Move 8-bit value in memory whose address is stored in a register to another
// register. Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV8_REGPTR_REG 0x0031
// Move 16-bit value in memory whose address is stored in a register to another
// register. Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV16_REGPTR_REG 0x0032
// Move 32-bit value in memory whose address is stored in a register to another
// register. Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV32_REGPTR_REG 0x0033
// Move 64-bit value in memory whose address is stored in a register to another
// register. Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV64_REGPTR_REG 0x0034

// Move value in one register to another. Syntax: `mov <register: u8> <register:
// u8>`
#define OP_MOV_REG_REG 0x0037

// Move value in register to memory address stores in another register. Syntax:
// `mov <register: u8> <register: u8>`
#define OP_MOV_REG_REGPTR 0x003A
// Move 8-bit value in register to memory address stores in another register.
// Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV8_REG_REGPTR 0x003B
// Move 16-bit value in register to memory address stores in another register.
// Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV16_REG_REGPTR 0x003C
// Move 32-bit value in register to memory address stores in another register.
// Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV32_REG_REGPTR 0x003D
// Move 64-bit value in register to memory address stores in another register.
// Syntax: `mov <register: u8> <register: u8>`
#define OP_MOV64_REG_REGPTR 0x003E

// Computer bitwise AND of register & literal, and store in register.
// Syntax: `and <register: u8> <data: word>`
#define OP_AND_REG_LIT 0x0040
// Computer 8-bit bitwise AND of register & literal, and store in register.
// Syntax: `and <register: u8> <data: 8-bit>`
#define OP_AND8_REG_LIT 0x0041
// Computer 16-bit bitwise AND of register & literal, and store in register.
// Syntax: `and <register: u8> <data: 16-bit>`
#define OP_AND16_REG_LIT 0x0042
// Computer 32-bit bitwise AND of register & literal, and store in register.
// Syntax: `and <register: u8> <data: 32-bit>`
#define OP_AND32_REG_LIT 0x0043
// Computer 64-bit bitwise AND of register & literal, and store in register.
// Syntax: `and <register: u8> <data: 64-bit>`
#define OP_AND64_REG_LIT 0x0044

// Computer bitwise AND of register1 & register2, and store in register1.
// Syntax: `and <register: u8> <register: u8>`
#define OP_AND_REG_REG 0x0045

// Computer bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: word>`
#define OP_OR_REG_LIT 0x0047
// Computer 8-bit bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: 8-bit>`
#define OP_OR8_REG_LIT 0x0048
// Computer 16-bit bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: 16-bit>`
#define OP_OR16_REG_LIT 0x0049
// Computer 32-bit bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: 32-bit>`
#define OP_OR32_REG_LIT 0x004A
// Computer 64-bit bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: 64-bit>`
#define OP_OR64_REG_LIT 0x004B

// Computer bitwise OR of register1 | register2, and store in register1.
// Syntax: `or <register: u8> <register: u8>`
#define OP_OR_REG_REG 0x004C

// Computer bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: word>`
#define OP_XOR_REG_LIT 0x0050
// Computer 8-bit bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: word>`
#define OP_XOR8_REG_LIT 0x0051
// Computer 16-bit bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: word>`
#define OP_XOR16_REG_LIT 0x0052
// Computer 32-bit bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: word>`
#define OP_XOR32_REG_LIT 0x0053
// Computer 64-bit bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: word>`
#define OP_XOR64_REG_LIT 0x0054

// Computer bitwise XOR of register1 ^ register2, and store in register1.
// Syntax: `xor <register: u8> <register: u8>`
#define OP_XOR_REG_REG 0x0055

// Computer bitwise NOT of register, and stores back in register.
// Syntax: `not <register: u8>`
#define OP_NOT 0x0056

// Negates value of register: x -> -x
// Syntax: `neg <register: u8>`
#define OP_NEG 0x0057
// Negates value of register AS A FLOAT: x -> -x
// Syntax: `negf32 <register: u8>`
#define OP_NEGF32 0x0058
// Negates value of register AS A DOUBLE: x -> -x
// Syntax: `negf64 <register: u8>`
#define OP_NEGF64 0x0059

// Logically shifts right (fills with 0) register by a literal amount.
// Syntax: `slr <reg: u8> <lit: u8>`
#define OP_LRSHIFT_LIT 0x005A
// Logically shifts right (fills with 0) register by a registers' value.
// Syntax: `slr <reg: u8> <reg: u8>`
#define OP_LRSHIFT_REG 0x005B

// Arithmetically shifts right (preserves sign) register by a literal amount.
// Syntax: `sar <reg: u8> <lit: u8>`
#define OP_ARSHIFT_LIT 0x005C
// Arithmetically shifts right (preserves sign) register by a register' value.
// Syntax: `sar <reg: u8> <reg: u8>`
#define OP_ARSHIFT_REG 0x005D

// Shifts left register by a literal amount.
// Syntax: `sll <reg: u8> <lit: u8>`
#define OP_LSHIFT_LIT 0x005E
// Shifts left register by a register' value.
// Syntax: `sll <reg: u8> <reg: u8>`
#define OP_LSHIFT_REG 0x005F

// Convert i8 to i16 in register.
// Syntax: `ci8i16 <reg: u8>`
#define OP_CVT_i8_i16 0x0060
// Convert i16 to i8 in register.
// Syntax: `ci16i8 <reg: u8>`
#define OP_CVT_i16_i8 0x0061
// Convert i16 to i32 in register.
// Syntax: `ci16i32 <reg: u8>`
#define OP_CVT_i16_i32 0x0062
// Convert i32 to i16 in register.
// Syntax: `ci32i16 <reg: u8>`
#define OP_CVT_i32_i16 0x0063
// Convert i32 to i64 in register.
// Syntax: `ci32i64 <reg: u8>`
#define OP_CVT_i32_i64 0x0064
// Convert i64 to i32 in register.
// Syntax: `ci64i32 <reg: u8>`
#define OP_CVT_i64_i32 0x0065

// Convert i32 to f32 in register.
// Syntax: `ci32f32 <reg: u8>`
#define OP_CVT_i32_f32 0x006A
// Convert f32 to i32 in register.
// Syntax: `cf32i32 <reg: u8>`
#define OP_CVT_f32_i32 0x006B
// Convert i64 to f64 in register.
// Syntax: `ci64f64 <reg: u8>`
#define OP_CVT_i64_f64 0x006C
// Convert f64 to i64 in register.
// Syntax: `cf64i64 <reg: u8>`
#define OP_CVT_f64_i64 0x006D

// Add a literal to a register : reg = reg + lit
// Syntax: `add <reg: u8> <lit: word>`
#define OP_ADD_REG_LIT 0x0070
// Add a literal to a register AS FLOATS : reg = reg + lit
// Syntax: `addf <reg: u8> <lit: f32>`
#define OP_ADDF32_REG_LIT 0x0071
// Add a literal to a register AS DOUBLES : reg = reg + lit
// Syntax: `addd <reg: u8> <lit: f64>`
#define OP_ADDF64_REG_LIT 0x0072
// Add a 2 registers together : r1 = r1 + r2
// Syntax: `add <reg: u8> <reg: u8>`
#define OP_ADD_REG_REG 0x0073
// Add a 2 registers together AS FLOATS : r1 = r1 + r2
// Syntax: `addf <reg: u8> <reg: u8>`
#define OP_ADDF32_REG_REG 0x0074
// Add a 2 registers together AS DOUBLES : r1 = r1 + r2
// Syntax: `addd <reg: u8> <reg: u8>`
#define OP_ADDF64_REG_REG 0x0075

#endif