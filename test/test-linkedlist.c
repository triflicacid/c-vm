#include <stdio.h>

#include "../src/linked-list.h"

struct Item {
    int a;
    int b;
};

LL_CREATE_NODET(Item, struct Item);

LL_CREATE_FINSERT(Item, struct Item);
LL_CREATE_FPRINT(Item, struct Item,
                 printf("a = %i; b = %i;\n", curr->data.a, curr->data.b));
LL_CREATE_FDESTROY(Item, );
LL_CREATE_FSIZE(Item);

int main() {
    struct LLNode_Item *head = 0;

    for (int i = 0; i < 5; ++i) {
        struct Item item = {.a = i, .b = i * i};
        linked_list_insert_Item(item, &head, i);
    }

    printf("Size: %i\n", linked_list_size_Item(head));
    linked_list_print_Item(head);

    linked_list_destroy_Item(&head);
    printf("DESTROYED.\n");

    printf("Size: %i\n", linked_list_size_Item(head));
    linked_list_print_Item(head);

    printf("Done.\n");

    return 0;
}