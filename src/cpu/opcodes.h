#ifndef __OPCODES_H__
#define __OPCODES_H__

#define OPCODE_T T_u16
#define OPCODE_T_FLAG "%u"

#include "cpu.h"

// No-operation, skip
#define OP_NOP 0x0000
// HALT
#define OP_HALT 0xFFFF

// DEBUG: PRINT MEMORY
#define OP_PMEM 0xFFFA
// DEBUG: PRINT REGISTERS
#define OP_PREG 0xFFFB
// DEBUG: Print contents of the stack as hexadecimal
// Syntax: `prs`
#define OP_PSTACK 0xFFFC

// Print hex `bytes` bytes from memory
// Syntax: `prh <bytes: u8> <addr: uword>`
#define OP_PRINT_HEX_MEM 0xFFE0
// Print hex `bytes` bytes from register
// Syntax: `prh <reg: u8>`
#define OP_PRINT_HEX_REG 0xFFE1

// Print `bytes` characters from memory
// Syntax: `prc <bytes: u8> <addr: uword>`
#define OP_PRINT_CHARS_MEM 0xFFE2
// Print up to four characters from a register. Print up until '\0' is found.
// Syntax: `prc <reg: u8>`
#define OP_PRINT_CHARS_REG 0xFFE3
// Print `bytes` characters from a literal
// Syntax: `prc <bytes: u8> <lit: ...>`
#define OP_PRINT_CHARS_LIT 0xFFE4

// Get character from STDIN and write to register
// Syntax: `inp <reg: u8>`
#define OP_GET_CHAR 0xFFD0

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
// Syntax: `mov <bytes: u8> <address: unsigned word> <data: n bytes>`
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

// Computer bitwise AND of `byte` long buffers at two memory addresses.
// Syntax: `and <bytes: u8> <addr1: uword> <addr2: uword>`
#define OP_AND_MEM_MEM 0x0046

// Computer bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: word>`
#define OP_OR_REG_LIT 0x0050
// Computer 8-bit bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: 8-bit>`
#define OP_OR8_REG_LIT 0x0051
// Computer 16-bit bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: 16-bit>`
#define OP_OR16_REG_LIT 0x0052
// Computer 32-bit bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: 32-bit>`
#define OP_OR32_REG_LIT 0x0053
// Computer 64-bit bitwise OR of register | literal, and store in register.
// Syntax: `or <register: u8> <data: 64-bit>`
#define OP_OR64_REG_LIT 0x0054

// Computer bitwise OR of register1 | register2, and store in register1.
// Syntax: `or <register: u8> <register: u8>`
#define OP_OR_REG_REG 0x0055

// Computer bitwise OR of `byte` long buffers at two memory addresses.
// Syntax: `or <bytes: u8> <addr1: uword> <addr2: uword>`
#define OP_OR_MEM_MEM 0x0056

// Computer bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: word>`
#define OP_XOR_REG_LIT 0x0060
// Computer 8-bit bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: u8>`
#define OP_XOR8_REG_LIT 0x0061
// Computer 16-bit bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: u16>`
#define OP_XOR16_REG_LIT 0x0062
// Computer 32-bit bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: u32>`
#define OP_XOR32_REG_LIT 0x0063
// Computer 64-bit bitwise XOR of register ^ literal, and store in register.
// Syntax: `xor <register: u8> <data: u64>`
#define OP_XOR64_REG_LIT 0x0064

// Computer bitwise XOR of register1 ^ register2, and store in register1.
// Syntax: `xor <register: u8> <register: u8>`
#define OP_XOR_REG_REG 0x0065

// Computer bitwise XOR of `byte` long buffers at two memory addresses.
// Syntax: `xor <bytes: u8> <addr1: uword> <addr2: uword>`
#define OP_XOR_MEM_MEM 0x0066

// Computer bitwise NOT of register, and stores back in register.
// Syntax: `not <register: u8>`
#define OP_NOT_REG 0x0070
// Computer bitwise NOT of value `bytes` long starting at address.
// Syntax: `not <bytes: u8> <addr: unsigned wort>`
#define OP_NOT_MEM 0x0071

// Negates value of register: x -> -x
// Syntax: `neg <register: u8>`
#define OP_NEG 0x0075
// Negates value of register AS A FLOAT: x -> -x
// Syntax: `negf32 <register: u8>`
#define OP_NEGF32 0x0076
// Negates value of register AS A DOUBLE: x -> -x
// Syntax: `negf64 <register: u8>`
#define OP_NEGF64 0x0077

// Logically shifts right (fills with 0) register by a literal amount.
// Syntax: `slr <reg: u8> <lit: u8>`
#define OP_LRSHIFT_LIT 0x007A
// Logically shifts right (fills with 0) register by a registers' value.
// Syntax: `slr <reg: u8> <reg: u8>`
#define OP_LRSHIFT_REG 0x007B

// Arithmetically shifts right (preserves sign) register by a literal amount.
// Syntax: `sar <reg: u8> <lit: u8>`
#define OP_ARSHIFT_LIT 0x007C
// Arithmetically shifts right (preserves sign) register by a register' value.
// Syntax: `sar <reg: u8> <reg: u8>`
#define OP_ARSHIFT_REG 0x007D

// Shifts left register by a literal amount.
// Syntax: `sll <reg: u8> <lit: u8>`
#define OP_LLSHIFT_LIT 0x007E
// Shifts left register by a register' value.
// Syntax: `sll <reg: u8> <reg: u8>`
#define OP_LLSHIFT_REG 0x007F

// Convert i8 to i16 in register.
// Syntax: `ci8i16 <reg: u8>`
#define OP_CVT_i8_i16 0x0080
// Convert i16 to i8 in register.
// Syntax: `ci16i8 <reg: u8>`
#define OP_CVT_i16_i8 0x0081
// Convert i16 to i32 in register.
// Syntax: `ci16i32 <reg: u8>`
#define OP_CVT_i16_i32 0x0082
// Convert i32 to i16 in register.
// Syntax: `ci32i16 <reg: u8>`
#define OP_CVT_i32_i16 0x0083
// Convert i32 to i64 in register.
// Syntax: `ci32i64 <reg: u8>`
#define OP_CVT_i32_i64 0x0084
// Convert i64 to i32 in register.
// Syntax: `ci64i32 <reg: u8>`
#define OP_CVT_i64_i32 0x0085

// Convert i32 to f32 in register.
// Syntax: `ci32f32 <reg: u8>`
#define OP_CVT_i32_f32 0x008A
// Convert f32 to i32 in register.
// Syntax: `cf32i32 <reg: u8>`
#define OP_CVT_f32_i32 0x008B
// Convert i64 to f64 in register.
// Syntax: `ci64f64 <reg: u8>`
#define OP_CVT_i64_f64 0x008C
// Convert f64 to i64 in register.
// Syntax: `cf64i64 <reg: u8>`
#define OP_CVT_f64_i64 0x008D

// Add a literal to a register : reg = reg + lit
// Syntax: `add <reg: u8> <lit: word>`
#define OP_ADD_REG_LIT 0x0090
// Add a literal to a register AS FLOATS : reg = reg + lit
// Syntax: `addf32 <reg: u8> <lit: f32>`
#define OP_ADDF32_REG_LIT 0x0091
// Add a literal to a register AS DOUBLES : reg = reg + lit
// Syntax: `addf64 <reg: u8> <lit: f64>`
#define OP_ADDF64_REG_LIT 0x0092
// Add a 2 registers together : r1 = r1 + r2
// Syntax: `add <reg: u8> <reg: u8>`
#define OP_ADD_REG_REG 0x0093
// Add a 2 registers together AS FLOATS : r1 = r1 + r2
// Syntax: `addf32 <reg: u8> <reg: u8>`
#define OP_ADDF32_REG_REG 0x0094
// Add a 2 registers together AS DOUBLES : r1 = r1 + r2
// Syntax: `addf64 <reg: u8> <reg: u8>`
#define OP_ADDF64_REG_REG 0x0095
// Add two `byte`-length buffers together. Indicate carry.
// Syntax: `add <bytes: u8> <addr1: uword> <addr2: uword>`
#define OP_ADD_MEM_MEM 0x0096

// Subtract a literal from a register : reg = reg - lit
// Syntax: `sub <reg: u8> <lit: word>`
#define OP_SUB_REG_LIT 0x00A0
// Subtract a literal from a register AS FLOATS : reg = reg - lit
// Syntax: `subf32 <reg: u8> <lit: f32>`
#define OP_SUBF32_REG_LIT 0x00A1
// Subtract a literal from a register AS DOUBLES : reg = reg - lit
// Syntax: `subf64 <reg: u8> <lit: f64>`
#define OP_SUBF64_REG_LIT 0x00A2
// Subtract 2 registers : r1 = r1 - r2
// Syntax: `sub <reg: u8> <reg: u8>`
#define OP_SUB_REG_REG 0x00A3
// Subtract 2 registers AS FLOATS : r1 = r1 - r2
// Syntax: `subf32 <reg: u8> <reg: u8>`
#define OP_SUBF32_REG_REG 0x00A4
// Subtract 2 registers AS DOUBLES : r1 = r1 - r2
// Syntax: `subf64 <reg: u8> <reg: u8>`
#define OP_SUBF64_REG_REG 0x00A5

// Multiply a literal and a register : reg = reg * lit
// Syntax: `mul <reg: u8> <lit: word>`
#define OP_MUL_REG_LIT 0x00B0
// Multiply a literal and a register AS FLOATS : reg = reg * lit
// Syntax: `mulf32 <reg: u8> <lit: f32>`
#define OP_MULF32_REG_LIT 0x00B1
// Multiply a literal and a register AS DOUBLES : reg = reg * lit
// Syntax: `mulf64 <reg: u8> <lit: f64>`
#define OP_MULF64_REG_LIT 0x00B2
// Multiply 2 registers : r1 = r1 * r2
// Syntax: `mul <reg: u8> <reg: u8>`
#define OP_MUL_REG_REG 0x00B3
// Multiply 2 registers AS FLOATS : r1 = r1 * r2
// Syntax: `mulf32 <reg: u8> <reg: u8>`
#define OP_MULF32_REG_REG 0x00B4
// Multiply 2 registers AS DOUBLES : r1 = r1 * r2
// Syntax: `mulf64 <reg: u8> <reg: u8>`
#define OP_MULF64_REG_REG 0x00B5

// Divide a register by a literal. Place remainder in REG_FLAG.
// Syntax: `div <reg: u8> <lit: word>`
#define OP_DIV_REG_LIT 0x00C0
// Divide a register by a literal.
// Syntax: `divf32 <reg: u8> <lit: f32>`
#define OP_DIVF32_REG_LIT 0x00C1
// Divide a register by a literal.
// Syntax: `divf64 <reg: u8> <lit: f64>`
#define OP_DIVF64_REG_LIT 0x00C2
// Divide 2 registers : r1 = r1 / r2. Place remainder in REG_FLAG.
// Syntax: `div <reg: u8> <reg: u8>`
#define OP_DIV_REG_REG 0x00C3
// Divide 2 registers : r1 = r1 / r2.
// Syntax: `divf32 <reg: u8> <reg: u8>`
#define OP_DIVF32_REG_REG 0x00C4
// Divide 2 registers : r1 = r1 / r2.
// Syntax: `divf64 <reg: u8> <reg: u8>`
#define OP_DIVF64_REG_REG 0x00C5

// Compare two registers, place result in REG_FLAG
// Syntax: `cmp <r1: u8> <r2: u8>`
#define OP_CMP_REG_REG 0x00D0
// Compare two registers, place result in REG_FLAG
// Syntax: `cmpf32 <r1: u8> <r2: u8>`
#define OP_CMPF32_REG_REG 0x00D1
// Compare two registers, place result in REG_FLAG
// Syntax: `cmpf64 <r1: u8> <r2: u8>`
#define OP_CMPF64_REG_REG 0x00D2
// Compare register to a literal, place result in REG_FLAG
// Syntax: `cmp <r1: u8> <lit: uword>`
#define OP_CMP_REG_LIT 0x00D3
// Compare register to a literal, place result in REG_FLAG
// Syntax: `cmpf32 <r1: u8> <lit: f32>`
#define OP_CMPF32_REG_LIT 0x00D4
// Compare register to a literal, place result in REG_FLAG
// Syntax: `cmpf64 <r1: u8> <lit: f64>`
#define OP_CMPF64_REG_LIT 0x00D5
// Compare two n-byte buffers, place result in REG_FLAG
// Syntax: `cmp <bytes: u8> <addr1: uword> <addr2: uword>`
#define OP_CMP_MEM_MEM 0x00DA

// Jump execution to provided memory address (set IP)
// Syntax: `jmp <addr: uword>`
#define OP_JMP_LIT 0x00E0
// Jump execution to address in register (set IP)
// Syntax: `jmp <reg: u8>`
#define OP_JMP_REG 0x00E1
// Jump execution to provided memory address IF comparison is CMP_EQ (set IP)
// Syntax: `jeq <addr: uword>`
#define OP_JMP_EQ_LIT 0x00E2
// Jump execution to address in register IF comparison is CMP_EQ (set IP)
// Syntax: `jeq <reg: u8>`
#define OP_JMP_EQ_REG 0x00E3
// Jump execution to provided memory address IF comparison is CMP_GT (set IP)
// Syntax: `jgt <addr: uword>`
#define OP_JMP_GT_LIT 0x00E4
// Jump execution to address in register IF comparison is CMP_GT (set IP)
// Syntax: `jgt <reg: u8>`
#define OP_JMP_GT_REG 0x00E5
// Jump execution to provided memory address IF comparison is CMP_LT (set IP)
// Syntax: `jlt <addr: uword>`
#define OP_JMP_LT_LIT 0x00E6
// Jump execution to address in register IF comparison is CMP_LT (set IP)
// Syntax: `jlt <reg: u8>`
#define OP_JMP_LT_REG 0x00E7
// Jump execution to provided memory address IF comparison is not CMP_EQ (set
// IP) Syntax: `jne <addr: uword>`
#define OP_JMP_NEQ_LIT 0x00E8
// Jump execution to address in register IF comparison is not CMP_EQ (set IP)
// Syntax: `jne <reg: u8>`
#define OP_JMP_NEQ_REG 0x00E9

// Push literal to the stack
// Syntax: `psh <lit: word>`
#define OP_PUSH_LIT 0x0100
// Push 8-bit literal to the stack
// Syntax: `psh <lit: u8>`
#define OP_PUSH8_LIT 0x0101
// Push 16-bit literal to the stack
// Syntax: `psh <lit: u16>`
#define OP_PUSH16_LIT 0x0102
// Push 32-bit literal to the stack
// Syntax: `psh <lit: u32>`
#define OP_PUSH32_LIT 0x0103
// Push 64-bit literal to the stack
// Syntax: `psh <lit: u64>`
#define OP_PUSH64_LIT 0x0104
// Push (n*8)-bit literal to the stack
// Syntax: `psh <bytes: u8> <lit: ...>`
#define OP_PUSHN_LIT 0x0105
// Push value at given address to the stack
// Syntax: `psh <addr: uword>`
#define OP_PUSH_MEM 0x0106
// Push 8-bit value at given address to the stack
// Syntax: `psh8 <addr: uword>`
#define OP_PUSH8_MEM 0x0107
// Push 16-bit value at given address to the stack
// Syntax: `psh16 <addr: uword>`
#define OP_PUSH16_MEM 0x0108
// Push 32-bit value at given address to the stack
// Syntax: `psh32 <addr: uword>`
#define OP_PUSH32_MEM 0x0109
// Push 64-bit value at given address to the stack
// Syntax: `psh64 <addr: uword>`
#define OP_PUSH64_MEM 0x010A
// Push k-bit value at given address to the stack
// Syntax: `psh <bytes: u8> <addr: ...>`
#define OP_PUSHN_MEM 0x010B
// Push register to the stack
// Syntax: `psh <reg: u8>`
#define OP_PUSH_REG 0x010C
// Push first 8-bits of a register to the stack
// Syntax: `psh8 <reg: u8>`
#define OP_PUSH8_REG 0x010D
// Push first 16-bits of a register to the stack
// Syntax: `psh16 <reg: u8>`
#define OP_PUSH16_REG 0x010E
// Push first 32-bits of a register to the stack
// Syntax: `psh32 <reg: u8>`
#define OP_PUSH32_REG 0x010F
// Push first 64-bits of a register to the stack
// Syntax: `psh64 <reg: u8>`
#define OP_PUSH64_REG 0x0110
// Push value at memory address stored in register to the stack
// Syntax: `psh <regptr: u8>`
#define OP_PUSH_REGPTR 0x0111
// Push 8-bit value at memory address stored in register to the stack
// Syntax: `psh8 <regptr: u8>`
#define OP_PUSH8_REGPTR 0x0112
// Push 16-bit value at memory address stored in register to the stack
// Syntax: `psh16 <regptr: u8>`
#define OP_PUSH16_REGPTR 0x0113
// Push 32-bit value at memory address stored in register to the stack
// Syntax: `psh32 <regptr: u8>`
#define OP_PUSH32_REGPTR 0x0114
// Push 64-bit value at memory address stored in register to the stack
// Syntax: `psh64 <regptr: u8>`
#define OP_PUSH64_REGPTR 0x0115
// Push (n*8)-bit value at memory address stored in register to the stack
// Syntax: `psh <bytes: u8> <regptr: ...>`
#define OP_PUSHN_REGPTR 0x0116

// Pop word from stack and place in register
// Syntax: `pop <reg: u8>`
#define OP_POP_REG 0x0120
// Pop 8 bits from stack and place in register
// Syntax: `pop8 <reg: u8>`
#define OP_POP8_REG 0x0121
// Pop 16 bits from stack and place in register
// Syntax: `pop16 <reg: u8>`
#define OP_POP16_REG 0x0122
// Pop 32 bits from stack and place in register
// Syntax: `pop32 <reg: u8>`
#define OP_POP32_REG 0x0123
// Pop 64 bits from stack and place in register
// Syntax: `pop64 <reg: u8>`
#define OP_POP64_REG 0x0124
// Pop (n*8)-bits from stack and write to memory address
// Syntax: `pop <bytes: u8> <addr: uword>`
#define OP_POPN_MEM 0x0125
// Pop word from the stack and place in address in register
// Syntax: `pop <regptr: u8>`
#define OP_POP_REGPTR 0x0126
// Pop 8-bit value from the stack and place in address in register
// Syntax: `pop8 <regptr: u8>`
#define OP_POP8_REGPTR 0x0127
// Pop 16-bit value from the stack and place in address in register
// Syntax: `pop16 <regptr: u8>`
#define OP_POP16_REGPTR 0x0128
// Pop 32-bit value from the stack and place in address in register
// Syntax: `pop32 <regptr: u8>`
#define OP_POP32_REGPTR 0x0129
// Pop 64-bit value from the stack and place in address in register
// Syntax: `pop64 <regptr: u8>`
#define OP_POP64_REGPTR 0x012A
// Pop (n*8)-bit value from the stack and place in address in register
// Syntax: `pop <bytes: u8> <regptr: u8>`
#define OP_POPN_REGPTR 0x012B

// Call literal
// Syntax: `cal <lit: uword>`
#define OP_CALL_LIT 0x0130
// Call value in register
// Syntax: `cal <reg: u8>`
#define OP_CALL_REG 0x0131

// Return from subroutine
// Syntax: `ret`
#define OP_RET 0x0139

#endif