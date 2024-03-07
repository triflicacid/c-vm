#include "string-list.h"
#include <string.h>
#include <stdio.h>

LL_CREATE_FINSERT(String, char *)

LL_CREATE_FINSERTNODE(String)

LL_CREATE_FFIND(String, char *, const char *, strcmp(curr->data, item) == 0)

LL_CREATE_FDESTROY(String, free(curr->data))

LL_CREATE_FPRINT(String, char *, printf("'%s'", curr->data))