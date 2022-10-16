#include "labels.h"

#include <stdio.h>
#include <string.h>

#include "../linked-list.h"

struct AsmLabel label_create(const char *string) {
    struct AsmLabel label;
    label.len = strlen(string);
    label.addr = 0;
    label.ptr = malloc(label.len);
    memcpy(label.ptr, string, label.len + 1);  // Include '\0'
    return label;
}

LL_CREATE_FPRINT(AsmLabel, struct AsmLabel,
                 printf("'%s'=%i\n", curr->data.ptr, curr->data.addr));

LL_CREATE_FINSERT(AsmLabel, struct AsmLabel);

LL_CREATE_FFIND(AsmLabel, struct AsmLabel, const char *,
                strcmp(curr->data.ptr, item) == 0);

LL_CREATE_FDESTROY(AsmLabel, free(curr->data.ptr));