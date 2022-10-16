#include "../src/assembler/labels.c"

int main() {
    struct LL_NODET_NAME(AsmLabel) *head = 0;
    linked_list_print_AsmLabel(head);

    struct AsmLabel label = label_create("a");
    label.addr = 1;
    linked_list_insert_AsmLabel(label, &head, -1);

    label = label_create("b");
    label.addr = 2;
    linked_list_insert_AsmLabel(label, &head, 0);

    label = label_create("c");
    label.addr = 3;
    linked_list_insert_AsmLabel(label, &head, -1);
    linked_list_print_AsmLabel(head);

    struct AsmLabel *result = linked_list_find_AsmLabel(head, "a");
    if (result == 0) {
        printf("No result.\n");
    } else {
        printf("Found: '%s'=%llu\n", result->ptr, result->addr);
    }

    printf("\nBefore: %p\n", head);
    linked_list_destroy_AsmLabel(&head);
    printf("After: %p\n", head);

    return 0;
}