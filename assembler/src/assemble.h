#ifndef ASM_ASSEMBLE_H_
#define ASM_ASSEMBLE_H_

#include <stdio.h>

#define ASM_MAX_LINE_LENGTH 1000
#define ASM_MAX_MNEMONIC_LENGTH 8
#define ASM_MAX_ARGS 5

// Not begun
#define ASM_STAGE_NONE 0
// Split into lines
#define ASM_STAGE_LINES 1
// Pre-Processed
#define ASM_STAGE_PREPROC 2
// Parsing AST
#define ASM_STAGE_PARSE 3
// Compiling
#define ASM_STAGE_COMPILE 4

// Is instruction/argument separator?
#define IS_SEPERATOR(c) (c == ',')

// If this character is encountered, create a new token
#define IS_TOKEN_SEP(c) \
    (c == ' ' || c == '\t' || c == '(' || c == '[' || c == '{' || c == ',')

// Write instruction to machine code with 1 argument. `offset` and `instruct`
// must be declared.
#define WRITE_INST1(opcode, type)               \
    BUF_WRITEK(offset, OPCODE_T, opcode);       \
    BUF_WRITEK(offset + sizeof(OPCODE_T), type, \
               (type)instruct->args->data.data);

// Decode instruction in struct AsmInstruction *
#define DECODE_INST1(_opcode, type) \
    instruct->opcode = _opcode;     \
    instruct->bytes = sizeof(OPCODE_T) + sizeof(type);

// Write instruction to machine code with 2 arguments. `offset` and `instruct`
// must be declared.
#define WRITE_INST2(opcode, type1, type2)                        \
    BUF_WRITEK(offset, OPCODE_T, opcode);                        \
    BUF_WRITEK(offset + sizeof(OPCODE_T), type1,                 \
               (type1)instruct->args->data.data);                \
    BUF_WRITEK(offset + sizeof(OPCODE_T) + sizeof(type1), type2, \
               (type2)instruct->args->next->data.data);

// Decode instruction in struct AsmInstruction *
#define DECODE_INST2(_opcode, type1, type2) \
    instruct->opcode = _opcode;             \
    instruct->bytes = sizeof(OPCODE_T) + sizeof(type1) + sizeof(type2);

// Write instruction to machine code with 3 arguments. `offset` and `instruct`
// must be declared.
#define WRITE_INST3(opcode, type1, type2, type3)                          \
    BUF_WRITEK(offset, OPCODE_T, opcode);                                 \
    BUF_WRITEK(offset + sizeof(OPCODE_T), type1,                          \
               (type1)instruct->args->data.data);                         \
    BUF_WRITEK(offset + sizeof(OPCODE_T) + sizeof(type1), type2,          \
               (type2)instruct->args->next->data.data);                   \
    BUF_WRITEK(offset + sizeof(OPCODE_T) + sizeof(type1) + sizeof(type2), \
               type3, (type3)instruct->args->next->next->data.data);

// Decode instruction in struct AsmInstruction *
#define DECODE_INST3(_opcode, type1, type2, type3) \
    instruct->opcode = _opcode;                    \
    instruct->bytes =                              \
        sizeof(OPCODE_T) + sizeof(type1) + sizeof(type2) + sizeof(type3);

#include "util.h"
#include "args.h"
#include "chunk.h"
#include "err.h"
#include "instruction.h"
#include "labels.h"
#include "line.h"
#include "symbol.h"
#include "processor/src/binary_header.h"

struct AsmData {
    int stage;                 // Stage of assembly
    unsigned long long bytes;  // Bytes used (populated in ASM_STAGE_PARSE)
    struct LL_NODET_NAME(AsmLine) * lines;      // Source lines
    struct LL_NODET_NAME(AsmSymbol) * symbols;  // Symbols (constants)
    struct LL_NODET_NAME(AsmLabel) * labels;    // Labels
    struct LL_NODET_NAME(AsmChunk) * chunks;    // Chunks (writable data)
    struct binary_header_data head_data;
};

// Create `struct AsmData`
struct AsmData asm_data_create();

// Destroy contents of `struct AsmData` (DOES NOT destroy the struct itself)
void asm_data_destroy(struct AsmData *data);

/** Given a file, return a linked list of source lines */
void asm_read_lines(FILE *fp, struct AsmData *data, struct AsmError *err);

/** Given linked list of lines, combine and insert into buffer. Return buffer
 * size (buffer IS null-terminated). */
unsigned long long asm_write_lines(struct LL_NODET_NAME(AsmLine) * lines,
                                   char **buffer);

/** Given a linked list of lines, pre-process them */
void asm_preprocess(struct AsmData *data, struct AsmError *err);

/** Given linked list of lines, parse it to an AST */
void asm_parse(struct AsmData *data, struct AsmError *err);

/** Given instruction AST, compile to a byte buffer. Return size of buffer, and set arg. Return 0 (null) if data.bytes==0 or there is an
 * error. */
size_t asm_compile(struct AsmData *data, struct AsmError *err, char **buf);

/** Given an instruction, populate instruct->bytes and instruct->opcode. Return
 * error. */
int asm_decode_instruction(struct AsmInstruction *instruct);

/** Write instruction data to buffer. Instruction must fit in buffer. */
int asm_write_instruction(void *buf, unsigned long long offset,
                          struct AsmInstruction *instruct);

#endif
