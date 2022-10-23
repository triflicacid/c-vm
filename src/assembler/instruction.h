#ifndef __ASM_INSTRUCTION_H__
#define __ASM_INSTRUCTION_H__

#include "../cpu/opcodes.h"
#include "../linked-list.h"
#include "args.h"
#include "assemble.h"
#include "chunk.h"

struct AsmInstruction {
    char *mnemonic;      // Instruction mnemonic
    OPCODE_T opcode;     // Instructiom opcode (when decoded)
    unsigned int bytes;  // Byte-width of instruction (when decoded)
    struct LL_NODET_NAME(AsmArgument) * args;  // Linked list of arguments
};

/** Print details of an instruction */
void asm_print_instruction(struct AsmInstruction *instruct);

// Free instruction from a chunk (doesn't destroy the chunk itself)
void asm_free_instruction_chunk(struct AsmChunk *chunk);

#endif