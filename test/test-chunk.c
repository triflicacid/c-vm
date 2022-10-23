#include <stdio.h>

#include "../src/assembler/chunk.c"
#include "../src/assembler/chunk.h"

int main() {
    struct LLNode_AsmChunk *head = 0;
    linked_list_print_AsmChunk(head);

    struct LLNode_AsmChunk chunk1 = {.data = {.offset = 10, .bytes = 18},
                                     .next = 0};
    linked_list_insertnode_AsmChunk(&head, &chunk1);

    linked_list_print_AsmChunk(head);
    printf("\n");

    struct LLNode_AsmChunk chunk2 = {.data = {.offset = 30, .bytes = 10},
                                     .next = 0};
    linked_list_insertnode_AsmChunk(&head, &chunk2);

    linked_list_print_AsmChunk(head);
    printf("\n");

    struct LLNode_AsmChunk chunk3 = {.data = {.offset = 0, .bytes = 10},
                                     .next = 0};
    linked_list_insertnode_AsmChunk(&head, &chunk3);

    linked_list_print_AsmChunk(head);
    printf("\n");

    struct LLNode_AsmChunk chunk4 = {.data = {.offset = 0, .bytes = 22},
                                     .next = 0};
    linked_list_insertnode_AsmChunk(&head, &chunk4);

    linked_list_print_AsmChunk(head);
    printf("\n\n");

    struct AsmChunk *chunk = 0;
    chunk = asm_chunk_in_range(head, 45, 50);
    if (chunk == 0)
        printf("(null)\n");
    else
        asm_print_chunk(chunk);
}