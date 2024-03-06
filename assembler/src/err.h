#ifndef ASM_ERR_H_
#define ASM_ERR_H_

#define ASM_ERR_NONE 0

// General error
#define ASM_ERR_GENERIC 1

// Argument count exceeded
#define ASM_ERR_TOO_MANY_ARGS 2

// Unknown argument
#define ASM_ERR_ARG 3

// Assembler: invalid register
#define ASM_ERR_REG 4

// Assembler: invalid address
#define ASM_ERR_ADDR 5

// Assembler: chunk collision error
#define ASM_ERR_MEMORY 6

// Assembler: unknown mnemonic
#define ASM_ERR_MNEMONIC 7

// Assembler: invalid argument combination
#define ASM_ERR_BAD_ARGS 8

// Assembler: cannot decode opcode
#define ASM_ERR_OPCODE 9

// Assembler: unknown directive
#define ASM_ERR_DIRECTIVE 10

// Assembler: unknown label encountered
#define ASM_ERR_UNKNOWN_LABEL 11

// Assembler: invalid label name
#define ASM_ERR_INVALID_LABEL 12


// Assembler - contain error info
struct AsmError {
    unsigned int line;  // Assembly source line reached
    unsigned int col;   // Column reached
    int errc;           // Error code (if any)
    int print;          // Print errors
    int debug;          // Debug?
};

struct AsmError asm_error_create();

#endif