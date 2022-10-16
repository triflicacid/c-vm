#ifndef __ASM_INSTRUCTION_ARG_H__
#define __ASM_INSTRUCTION_ARG_H__

#include "../linked-list.h"

/** Structure representing an argument */
struct AsmArgument {
    unsigned char type;       // Argument type. Constant: `ASM_ARG_...`
    unsigned long long data;  // Argument data
};

LL_CREATE_NODET(AsmArgument, struct AsmArgument);

LL_DECL_FINSERT(AsmArgument, struct AsmArgument);

LL_DECL_FINSERTNODE(AsmArgument);

LL_DECL_FSIZE(AsmArgument);

#endif