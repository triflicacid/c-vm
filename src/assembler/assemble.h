#ifndef __ASSEMBLE_H__
#define __ASSEMBLE_H__

#include <stdio.h>

#include "../util.h"
#include "args.h"
#include "instruction.h"

#define ASM_MAX_LINE_LENGTH 1000
#define ASM_MAX_MNEMONIC_LENGTH 8
#define ASM_MAX_ARGS 5

#define ASM_ARG_LIT 0
#define ASM_ARG_ADDR 1
#define ASM_ARG_REG 2
#define ASM_ARG_REGPTR 3

// Is instruction/argument seperator?
#define IS_SEPERATOR(c) (c == ',')

// Write instruction to machine code with 1 argument
#define WRITE_INST1(opcode, type)                         \
    BUF_WRITEK(instruct->offset, OPCODE_T, opcode);       \
    BUF_WRITEK(instruct->offset + sizeof(OPCODE_T), type, \
               (type)instruct->args->data.data);

// Decode instruction in struct AsmInstruction *
#define DECODE_INST1(_opcode, type) \
    instruct->opcode = _opcode;     \
    instruct->bytes = sizeof(OPCODE_T) + sizeof(type);

// Write instruction to machine code with 2 arguments
#define WRITE_INST2(opcode, type1, type2)                                  \
    BUF_WRITEK(instruct->offset, OPCODE_T, opcode);                        \
    BUF_WRITEK(instruct->offset + sizeof(OPCODE_T), type1,                 \
               (type1)instruct->args->data.data);                          \
    BUF_WRITEK(instruct->offset + sizeof(OPCODE_T) + sizeof(type1), type2, \
               (type2)instruct->args->next->data.data);

// Decode instruction in struct AsmInstruction *
#define DECODE_INST2(_opcode, type1, type2) \
    instruct->opcode = _opcode;             \
    instruct->bytes = sizeof(OPCODE_T) + sizeof(type1) + sizeof(type2);

// Write instruction to machine code with 3 arguments
#define WRITE_INST3(opcode, type1, type2, type3)                             \
    BUF_WRITEK(instruct->offset, OPCODE_T, opcode);                          \
    BUF_WRITEK(instruct->offset + sizeof(OPCODE_T), type1,                   \
               (type1)instruct->args->data.data);                            \
    BUF_WRITEK(instruct->offset + sizeof(OPCODE_T) + sizeof(type1), type2,   \
               (type2)instruct->args->next->data.data);                      \
    BUF_WRITEK(                                                              \
        instruct->offset + sizeof(OPCODE_T) + sizeof(type1) + sizeof(type2), \
        type3, (type3)instruct->args->next->next->data.data);

// Decode instruction in struct AsmInstruction *
#define DECODE_INST3(_opcode, type1, type2, type3) \
    instruct->opcode = _opcode;                    \
    instruct->bytes =                              \
        sizeof(OPCODE_T) + sizeof(type1) + sizeof(type2) + sizeof(type3);

struct Assemble {
    unsigned int buf_offset;  // Final buffer offset reached
    unsigned int line;        // Assembly source line reached
    unsigned int col;         // Column reached
    int errc;                 // Error code (if any)
};

/** Assemble file into a buffer */
struct Assemble assemble(FILE *fp, void *buf, unsigned int buf_size,
                         unsigned int print_errors, int debug);

/** Given an instruction, populate instruct->bytes and instruct->opcode. Return
 * error. */
int decode_instruction(struct AsmInstruction *instruct);

/** Write instruction data to buffer. Instruction must fit in buffer. */
int write_instruction(void *buf, struct AsmInstruction *instruct);

/** Print linked list of AsmInstruction */
void asm_print_instruction_list(struct LL_NODET_NAME(AsmInstruction) * head);

/** Free linked list of AsmInstruction */
void asm_free_instruction_list(struct LL_NODET_NAME(AsmInstruction) * head);

#endif
