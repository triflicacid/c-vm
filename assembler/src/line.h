#ifndef ASM_LINE_H_
#define ASM_LINE_H_

#include "linked-list.h"

struct AsmLine {
    int n;      // Line number
    char *str;  // Line contents
    int len;    // Line length
    char *note; // Note attached to the line
};

void destroy_asm_line(struct AsmLine *line);

LL_CREATE_NODET(AsmLine, struct AsmLine);

LL_DECL_FINSERTNODE(AsmLine)

LL_DECL_FDESTROY(AsmLine)

LL_DECL_FPRINT(AsmLine, struct AsmLine)

LL_DECL_FREMOVENODE(AsmLine)

void asm_line_print_note(struct AsmLine *line);

#endif