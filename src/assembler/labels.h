#ifndef __ASM_LABELS_H__
#define __ASM_LABELS_H__

#include "../linked-list.h"

// Structure for describing a label
struct AsmLabel {
    char *ptr;                // Pointer to string
    unsigned int len;         // Length of label string
    unsigned long long addr;  // Address
};

// Create label structure
struct AsmLabel label_create(const char *string);

LL_CREATE_NODET(AsmLabel, struct AsmLabel);

// Print label structure list
LL_DECL_FPRINT(AsmLabel, struct AsmLabel);

// Insert label structure into node list. If pos is -1, insert at end.
LL_DECL_FINSERT(AsmLabel, struct AsmLabel);

// Return pointer to LABEL with given label string, or NULL pointer.
LL_DECL_FFIND(AsmLabel, struct AsmLabel, const char *);

// Destroy linked list
LL_DECL_FDESTROY(AsmLabel);

#endif