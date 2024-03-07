#include "line.h"
#include "util.h"

#include <stdio.h>

void destroy_asm_line(struct AsmLine *line) {
    free(line->str);
    if (line->note)
        free(line->note);
}

LL_CREATE_FINSERTNODE(AsmLine)

LL_CREATE_FDESTROY(AsmLine, destroy_asm_line(&curr->data))

LL_CREATE_FPRINT(AsmLine, struct AsmLine,
                 printf("%.2u [%.2u] | %s \n", curr->data.n, curr->data.len, curr->data.str))

LL_CREATE_FREMOVENODE(AsmLine)

void asm_line_print_note(struct AsmLine *line) {
    if (line && line->note != NULL)
        printf(CONSOLE_BLUE "NOTE" CONSOLE_RESET " %s\n", line->note);
}
