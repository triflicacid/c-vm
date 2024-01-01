#ifndef __ASM_INSTRUCTION_ARG_H__
#define __ASM_INSTRUCTION_ARG_H__

#include "linked-list.h"

#define ASM_ARG_LIT 0
#define ASM_ARG_ADDR 1
#define ASM_ARG_REG 2
#define ASM_ARG_REGPTR 3
#define ASM_ARG_LABEL 4

/** Structure representing an argument */
struct AsmArgument {
    unsigned char type;       // Argument type. Constant: `ASM_ARG_...`
    unsigned long long data;  // Argument data
};

LL_CREATE_NODET(AsmArgument, struct AsmArgument);

LL_DECL_FINSERT(AsmArgument, struct AsmArgument)

LL_DECL_FINSERTNODE(AsmArgument)

LL_DECL_FSIZE(AsmArgument)

// Given an argument, print details (same line). Return error.
int print_asm_arg(struct AsmArgument *arg);

#endif