#ifndef __ASM_CHUNK_H__
#define __ASM_CHUNK_H__

#include "linked-list.h"

#define ASM_CHUNKT_UNKNOWN 0
#define ASM_CHUNKT_INSTRUCTION 1
#define ASM_CHUNKT_DATA 2

struct AsmChunk {
    unsigned long long offset;  // Byte offset
    unsigned int bytes;         // Byte length
    int type;                   // Type of thing in chunk
    void *data;                 // Data stored in chunk.
};

LL_CREATE_NODET(AsmChunk, struct AsmChunk);

LL_DECL_FPRINT(AsmChunk, struct AsmChunk)

void asm_print_chunk(struct AsmChunk *chunk);

void asm_destroy_chunk(struct AsmChunk *chunk);

// Insert chunk node into the linked list
void linked_list_insertnode_AsmChunk(struct LL_NODET_NAME(AsmChunk) * *chunks,
                                     struct LL_NODET_NAME(AsmChunk) * chunk);

// Is there a chunk infringing on this range? If so, return pointer to chunk
// that is in this range. Else, NULL
struct AsmChunk *asm_chunk_in_range(struct LL_NODET_NAME(AsmChunk) * chunks,
                                    unsigned long long start,
                                    unsigned long long end);

LL_DECL_FDESTROY(AsmChunk)

#endif