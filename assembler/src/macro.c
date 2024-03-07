#include "macro.h"
#include <string.h>

struct AsmMacro *create_macro() {
    struct AsmMacro *macro = malloc(sizeof(*macro));
    macro->name = NULL;
    macro->line_count = 0;
    macro->lines = NULL;
    macro->arg_count = 0;
    macro->args = NULL;

    return macro;
}

void destroy_macro(struct AsmMacro *macro) {
    free(macro->name);
    linked_list_destroy_String(&macro->args);
    linked_list_destroy_String(&macro->lines);
    free(macro);
}

LL_CREATE_FINSERT(AsmMacro, struct AsmMacro *)

LL_CREATE_FINSERTNODE(AsmMacro)

LL_CREATE_FFIND(AsmMacro, struct AsmMacro *, const char *,
                strcmp(curr->data->name, item) == 0)

LL_CREATE_FDESTROY(AsmMacro, destroy_macro(curr->data))
