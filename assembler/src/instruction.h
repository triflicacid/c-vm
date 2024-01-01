#ifndef ASM_INSTRUCTION_H_
#define ASM_INSTRUCTION_H_

#include "args.h"
#include "processor/src/opcodes.h"

struct AsmInstruction {
    char *mnemonic;      // Instruction mnemonic
    OPCODE_T opcode;     // Instruction opcode (when decoded)
    unsigned int bytes;  // Byte-width of instruction (when decoded)
    struct LL_NODET_NAME(AsmArgument) * args;  // Linked list of arguments
};

#include "linked-list.h"
#include "chunk.h"
#include "assemble.h"

/** Print details of an instruction */
void asm_print_instruction(struct AsmInstruction *instruct);

// Free instruction from a chunk (doesn't destroy the chunk itself)
void asm_free_instruction_chunk(struct AsmChunk *chunk);

#endif