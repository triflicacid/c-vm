#ifndef __ASM_STRING_LIST_H__
#define __ASM_STRING_LIST_H__

#include "linked-list.h"

LL_CREATE_NODET(String, char *);

LL_DECL_FINSERT(String, char *);

LL_DECL_FINSERTNODE(String)

LL_DECL_FFIND(String, char *, const char *)

LL_DECL_FDESTROY(String)

LL_DECL_FPRINT(String, char *)

#endif