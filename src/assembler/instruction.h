#ifndef __ASM_INSTRUCTION_H__
#define __ASM_INSTRUCTION_H__

#include "../cpu/opcodes.h"
#include "../linked-list.h"
#include "args.h"
#include "assemble.h"

struct AsmInstruction {
    char *mnemonic;             // Instruction mnemonic
    OPCODE_T opcode;            // Instructiom opcode (when decoded)
    unsigned long long offset;  // Instruction offset in memory
    unsigned int bytes;         // Byte-width of instruction (when decoded)
    struct LL_NODET_NAME(AsmArgument) * args;  // Linked list of arguments
};

LL_CREATE_NODET(AsmInstruction, struct AsmInstruction);

LL_DECL_FINSERT(AsmInstruction, struct AsmInstruction);

LL_DECL_FINSERTNODE(AsmInstruction);

#endif