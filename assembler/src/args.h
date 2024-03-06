#ifndef ASM_INSTRUCTION_ARG_H_
#define ASM_INSTRUCTION_ARG_H_

#include "linked-list.h"

#define ASM_ARG_LIT 0
#define ASM_ARG_ADDR 1
#define ASM_ARG_REG 2
#define ASM_ARG_REGPTR 3
#define ASM_ARG_LABEL_LIT 4
#define ASM_ARG_LABEL_ADDR 5

// Check if an argument's data can be used as a literal
#define ASM_ARG_IS_LIT(x) (x == ASM_ARG_LIT || x == ASM_ARG_LABEL_LIT)

// Check if an argument's data can be used as an address
#define ASM_ARG_IS_ADDR(x) (x == ASM_ARG_ADDR || x == ASM_ARG_LABEL_ADDR)

// Check if an argument is a label
#define ASM_ARG_IS_LABEL(x) (x == ASM_ARG_LABEL_ADDR || x == ASM_ARG_LABEL_LIT)

// Given label, return representing type e.g., [label] -> [n]
#define ASM_ARG_DEREF_LABEL(x) (x == ASM_ARG_LABEL_LIT ? ASM_ARG_LIT : ASM_ARG_ADDR)

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