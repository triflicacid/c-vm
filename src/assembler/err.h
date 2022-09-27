#ifndef __ASM_ERR_H__
#define __ASM_ERR_H__

#define ASM_ERR_NONE 0

// General error
#define ASM_ERR_GENERIC 1

// Unknown argument
#define ASM_ERR_ARG 2

// Assembler: invalid register
#define ASM_ERR_REG 3

// Assembler: invalid address
#define ASM_ERR_ADDR 4

// Asembler: out of memory
#define ASM_ERR_MEMORY 5

// Assembler: unknown mnemonic
#define ASM_ERR_MNEMONIC 6

// Assembler: invalid operand combination
#define ASM_ERR_OPERAND 7

#endif