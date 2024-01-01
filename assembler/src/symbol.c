#include "symbol.h"
#include <string.h>
#include <stdio.h>

LL_CREATE_FINSERT(AsmSymbol, struct AsmSymbol)

LL_CREATE_FINSERTNODE(AsmSymbol)

LL_CREATE_FFIND(AsmSymbol, struct AsmSymbol, const char *,
                strcmp(curr->data.name, item) == 0)

LL_CREATE_FDESTROY(AsmSymbol, {
    free(curr->data.name);
    free(curr->data.value);
})

LL_CREATE_FPRINT(AsmSymbol, struct AsmSymbol,
                 printf("'%s' = %s\n", curr->data.name, curr->data.value))