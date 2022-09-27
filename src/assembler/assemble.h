#ifndef __ASSEMBLE_H__
#define __ASSEMBLE_H__

#include <stdio.h>
#include <string.h>

#include "../util.h"

#define ASM_MAX_LINE_LENGTH 1000
#define ASM_MAX_MNEMONIC_LENGTH 15
#define ASM_MAX_ARGS 10
#define ASM_MAX_BYTES 1048576

#define ASM_ARG_LIT 0
#define ASM_ARG_ADDR 1
#define ASM_ARG_REG 2
#define ASM_ARG_REGPTR 3

#define IS_WHITESPACE(c) \
    (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',')

// Return if memory overflow
#define RET_MEMOV(bytes) \
    if (*buf_offset + bytes >= buf_size) return ASM_ERR_MEMORY;

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
struct Assemble assemble(FILE *fp, void *buf, unsigned int buf_size);

/** Convert a mnemonic and array of AsmArguments to machine code. Return erro
 * code. */
int decode_instruction(void *buf, unsigned int buf_size,
                       unsigned int *buf_offset, const char *mnemonic,
                       struct AsmArgument *args, unsigned int argc);

#endif
