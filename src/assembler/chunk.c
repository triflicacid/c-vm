#include "chunk.h"

#include "../cpu/bit-ops.h"
#include "instruction.h"

void asm_print_chunk(struct AsmChunk *chunk) {
    printf("Chunk at %llu of %u bytes", chunk->offset, chunk->bytes);
    switch (chunk->type) {
        case ASM_CHUNKT_INSTRUCTION:
            printf(" - instruction:\n\t");
            asm_print_instruction(chunk->data);
            break;
        case ASM_CHUNKT_DATA:
            printf(" - data:\n\t{");
            print_bytes(chunk->data, chunk->bytes);
            printf("\b}\n");
            break;
        default:
            printf("\n");
    }
}

void asm_destroy_chunk(struct AsmChunk *chunk) {
    switch (chunk->type) {
        case ASM_CHUNKT_INSTRUCTION:
            asm_free_instruction_chunk(chunk);
            break;
        case ASM_CHUNKT_DATA:
            free(chunk->data);
            break;
    }
}

LL_CREATE_FDESTROY(AsmChunk, asm_destroy_chunk(&(curr->data)));

LL_CREATE_FPRINT(AsmChunk, struct AsmChunk, asm_print_chunk(&(curr->data)));

void linked_list_insertnode_AsmChunk(struct LL_NODET_NAME(AsmChunk) * *chunks,
                                     struct LL_NODET_NAME(AsmChunk) * chunk) {
    if (*chunks == 0) {  // Empty
        *chunks = chunk;
    } else {
        struct LL_NODET_NAME(AsmChunk) *curr = *chunks, *last = 0, *prev = 0;
        while (curr != 0 &&
               curr->data.offset + curr->data.bytes <= chunk->data.offset) {
            if (curr->next == 0) last = curr;
            prev = curr;
            curr = curr->next;
        }
        if (last != 0) {
            last->next = chunk;
            chunk->next = 0;
        } else {
            if (curr == *chunks) *chunks = chunk;
            chunk->next = curr;
            if (prev) prev->next = chunk;
        }
    }
}

struct AsmChunk *asm_chunk_in_range(struct LL_NODET_NAME(AsmChunk) * chunks,
                                    unsigned long long start,
                                    unsigned long long end) {
    struct LL_NODET_NAME(AsmChunk) *curr = chunks;
    while (curr != 0) {
        if (curr->data.offset <= end &&
            curr->data.offset + curr->data.bytes > start) {
            return &(curr->data);
        }
        curr = curr->next;
    }
    return 0;
}