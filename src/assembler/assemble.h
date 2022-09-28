#ifndef __ASSEMBLE_H__
#define __ASSEMBLE_H__

#include <stdio.h>
#include <string.h>

#include "../util.h"

#define ASM_MAX_LINE_LENGTH 1000
#define ASM_MAX_MNEMONIC_LENGTH 8
#define ASM_MAX_ARGS 5

#define ASM_ARG_LIT 0
#define ASM_ARG_ADDR 1
#define ASM_ARG_REG 2
#define ASM_ARG_REGPTR 3

// Is instruction/argument seperator?
#define IS_SEPERATOR(c) \
    (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',')

// Return if memory overflow
#define RET_MEMOV(bytes) \
    if (*buf_offset + bytes >= buf_size) return ASM_ERR_MEMORY;

// Write instruction to machine code with 1 argument
#define WRITE_INST1(opcode, type)               \
    RET_MEMOV(sizeof(OPCODE_T) + sizeof(type)); \
    BUF_WRITE(*buf_offset, OPCODE_T, opcode);   \
    BUF_WRITE(*buf_offset, type, args[0].data);

// Write instruction to machine code with 2 arguments
#define WRITE_INST2(opcode, type1, type2)                        \
    RET_MEMOV(sizeof(OPCODE_T) + sizeof(type1) + sizeof(type2)); \
    BUF_WRITE(*buf_offset, OPCODE_T, opcode);                    \
    BUF_WRITE(*buf_offset, type1, args[0].data);                 \
    BUF_WRITE(*buf_offset, type2, args[1].data);

// Write instruction to machine code with 3 arguments
#define WRITE_INST3(opcode, type1, type2, type3)                 \
    RET_MEMOV(sizeof(OPCODE_T) + sizeof(type1) + sizeof(type2) + \
              sizeof(type3));                                    \
    BUF_WRITE(*buf_offset, OPCODE_T, opcode);                    \
    BUF_WRITE(*buf_offset, type1, args[0].data);                 \
    BUF_WRITE(*buf_offset, type2, args[1].data);                 \
    BUF_WRITE(*buf_offset, type2, args[2].data);

struct Assemble {
    unsigned int buf_offset;  // Final buffer offset reached
    unsigned int line;        // Assembly source line reached
    unsigned int col;         // Column reached
    int errc;                 // Error code (if any)
};

/** Structure representing an argument */
struct AsmArgument {
    T_u8 type;   // Argument type. Constant: `ASM_ARG_...`
    T_i64 data;  // Argument data
};

/** Assemble file into a buffer */
struct Assemble assemble(FILE *fp, void *buf, unsigned int buf_size,
                         unsigned int print_errors);

/** Convert a mnemonic and array of AsmArguments to machine code. Return erro
 * code. */
int decode_instruction(void *buf, unsigned int buf_size,
                       unsigned int *buf_offset, const char *mnemonic,
                       struct AsmArgument *args, unsigned int argc);

#endif
