#ifndef ASM_ERR_H_
#define ASM_ERR_H_

// No error
#define ASM_OK 0

// Generic syntax error
#define ASM_ERR_SYNTAX 1

// Assembler: unknown mnemonic
#define ASM_ERR_MNEMONIC 2

// Assembler: invalid argument combination
#define ASM_ERR_BAD_ARGS 3

// Assembler: unknown directive
#define ASM_ERR_DIRECTIVE 4

// Assembler: unknown label encountered
#define ASM_ERR_UNKNOWN_LABEL 5

// Assembler: invalid label name
#define ASM_ERR_INVALID_LABEL 6

// Assembler: generic internal error, should not happen when not in development.
#define ASM_ERR_INTERNAL 10

// Assembler: cannot decode opcode
#define ASM_ERR_OPCODE 11

// Assembler: chunk collision error
#define ASM_ERR_CHUNK 12


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