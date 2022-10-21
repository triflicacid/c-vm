#ifndef __ASM_SYMBOL_H__
#define __ASM_SYMBOL_H__

#include "../linked-list.h"

struct AsmSymbol {
    char *name;         // Symbol name
    char *value;        // Symbol value
};

LL_CREATE_NODET(AsmSymbol, struct AsmSymbol);

LL_DECL_FINSERT(AsmSymbol, struct AsmSymbol);

LL_DECL_FINSERTNODE(AsmSymbol);

LL_DECL_FFIND(AsmSymbol, struct AsmSymbol, const char *);

LL_DECL_FDESTROY(AsmSymbol);

LL_DECL_FPRINT(AsmSymbol, struct AsmSymbol);

#endif