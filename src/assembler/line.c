#include "line.h"

LL_CREATE_FINSERTNODE(AsmLine);

LL_CREATE_FDESTROY(AsmLine, free(curr->data.str));

LL_CREATE_FPRINT(AsmLine, struct AsmLine,
                 printf("%.2u [%.2u] | %s \n", curr->data.n, curr->data.len,
                        curr->data.str));

LL_CREATE_FREMOVENODE(AsmLine);