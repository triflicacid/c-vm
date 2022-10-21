#ifndef __ASM_LINE_H__
#define __ASM_LINE_H__

#include "../linked-list.h"

struct AsmLine {
    unsigned int n;    // Line number
    char *str;         // Line contents
    unsigned int len;  // Line length
};

LL_CREATE_NODET(AsmLine, struct AsmLine);

LL_DECL_FINSERTNODE(AsmLine);

LL_DECL_FDESTROY(AsmLine);

LL_DECL_FPRINT(AsmLine, struct AsmLine);

LL_DECL_FREMOVENODE(AsmLine);

#endif