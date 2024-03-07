#ifndef __ASM_MACRO_H__
#define __ASM_MACRO_H__

#include "linked-list.h"
#include "string-list.h"

struct AsmMacro {
    char *name;         // Macro name
    struct LL_NODET_NAME(String) *args;
    int arg_count;
    struct LL_NODET_NAME(String) *lines;
    int line_count;
};

/** Create empty macro */
struct AsmMacro *create_macro();

/** Destroy macro and its contents. */
void destroy_macro(struct AsmMacro *macro);

LL_CREATE_NODET(AsmMacro, struct AsmMacro *);

LL_DECL_FINSERT(AsmMacro, struct AsmMacro *)

LL_DECL_FINSERTNODE(AsmMacro)

LL_DECL_FFIND(AsmMacro, struct AsmMacro *, const char *)

LL_DECL_FDESTROY(AsmMacro)

#endif