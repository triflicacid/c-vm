#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <stdlib.h>

// Get linked list node name from name
#define LL_NODET_NAME(name) LLNode_##name

// Function to create linked list node from data type
#define LL_CREATE_NODET(name, datatype) \
    struct LLNode_##name {              \
        datatype data;                  \
        struct LLNode_##name *next;     \
    }

// Creates function: void linked_list_insert_<name>(<datatype> item, struct
// LLNode_<name> **head, int pos). Inserts `item` at this position. pos = -1
// means insert at the end.
#define LL_CREATE_FINSERT(name, datatype)                                      \
    void linked_list_insert_##name(datatype item, struct LLNode_##name **head, \
                                   int pos) {                                  \
        unsigned int size = 0;                                                 \
        for (struct LLNode_##name *c = *head; c != 0; ++size, c = c->next)     \
            ;                                                                  \
        if (pos >= size) pos = -1;                                             \
        if (*head == 0 || pos == 0) {                                          \
            struct LLNode_##name *node = malloc(sizeof(struct LLNode_##name)); \
            node->data = item;                                                 \
            node->next = *head;                                                \
            *head = node;                                                      \
        } else if (pos == -1) {                                                \
            struct LLNode_##name *curr = *head;                                \
            while (curr->next) curr = curr->next;                              \
            struct LLNode_##name *old_next = curr->next;                       \
            curr->next = malloc(sizeof(struct LLNode_##name));                 \
            curr->next->data = item;                                           \
            curr->next->next = old_next;                                       \
        } else {                                                               \
            struct LLNode_##name *bef = *head, *curr = *head;                  \
            for (int i = 0; curr->next && i < pos; i++) curr = curr->next;     \
            while (bef->next != curr) bef = bef->next;                         \
            struct LLNode_##name *node = malloc(sizeof(struct LLNode_##name)); \
            node->data = item;                                                 \
            node->next = curr;                                                 \
            bef->next = node;                                                  \
        }                                                                      \
    }

#define LL_DECL_FINSERT(name, datatype)                                        \
    void linked_list_insert_##name(datatype item, struct LLNode_##name **head, \
                                   int pos);

// Creates function: void linked_list_insertnode_<name>(struct LLNode_<name> *
// node, struct LLNode_<name> **head, int pos). Inserts `node` at this position.
// pos = -1 means insert at the end. NB node->next is overwritten.
#define LL_CREATE_FINSERTNODE(name)                                            \
    void linked_list_insertnode_##name(struct LLNode_##name *node,             \
                                       struct LLNode_##name **head, int pos) { \
        unsigned int size = 0;                                                 \
        for (struct LLNode_##name *c = *head; c != 0; ++size, c = c->next)     \
            ;                                                                  \
        if (pos >= size) pos = -1;                                             \
        if (*head == 0 || pos == 0) {                                          \
            node->next = *head;                                                \
            *head = node;                                                      \
        } else if (pos == -1) {                                                \
            struct LLNode_##name *curr = *head;                                \
            while (curr->next) curr = curr->next;                              \
            struct LLNode_##name *old_next = curr->next;                       \
            curr->next = node;                                                 \
            curr->next->next = old_next;                                       \
        } else {                                                               \
            struct LLNode_##name *bef = *head, *curr = *head;                  \
            for (int i = 0; curr->next && i < pos; i++) curr = curr->next;     \
            while (bef->next != curr) bef = bef->next;                         \
            node->next = curr;                                                 \
            bef->next = node;                                                  \
        }                                                                      \
    }

#define LL_DECL_FINSERTNODE(name)                                  \
    void linked_list_insertnode_##name(struct LLNode_##name *node, \
                                       struct LLNode_##name **head, int pos);

// Creates function: datatype *linked_list_find_<name>(struct LLNode_<name>
// *head, test_datatype item) which finds `item` if present based on a
// condition.
#define LL_CREATE_FFIND(name, datatype, test_datatype, condition) \
    datatype *linked_list_find_##name(struct LLNode_##name *head, \
                                      test_datatype item) {       \
        struct LLNode_##name *curr = head;                        \
        while (curr != 0) {                                       \
            if (condition) return &(curr->data);                  \
            curr = curr->next;                                    \
        }                                                         \
        return 0;                                                 \
    }

#define LL_DECL_FFIND(name, datatype, test_datatype)              \
    datatype *linked_list_find_##name(struct LLNode_##name *head, \
                                      test_datatype item);

// Creates function: void linked_list_print_<name>(struct LLNode_<name>
// *head) which prints each item of the list
#define LL_CREATE_FPRINT(name, datatype, action)                \
    void linked_list_print_##name(struct LLNode_##name *head) { \
        struct LLNode_##name *curr = head;                      \
        while (curr) {                                          \
            action;                                             \
            curr = curr->next;                                  \
        }                                                       \
    }

#define LL_DECL_FPRINT(name, datatype) \
    void linked_list_print_##name(struct LLNode_##name *head);

// Creates function: void linked_list_destroy_<name>(struct LLNode_<name>
// **head). `action` contains extra deletion action.
#define LL_CREATE_FDESTROY(name, action)                           \
    void linked_list_destroy_##name(struct LLNode_##name **head) { \
        struct LLNode_##name *curr = *head, *next = 0;             \
        while (curr) {                                             \
            next = curr->next;                                     \
            action;                                                \
            free(curr);                                            \
            curr = next;                                           \
        }                                                          \
        *head = 0;                                                 \
    }

#define LL_DECL_FDESTROY(name) \
    void linked_list_destroy_##name(struct LLNode_##name **head);

#define LL_DECL_FSIZE(name) \
    unsigned int linked_list_size_##name(struct LLNode_##name *head);

// Return size of linked list
#define LL_CREATE_FSIZE(name)                                          \
    unsigned int linked_list_size_##name(struct LLNode_##name *head) { \
        unsigned int size = 0;                                         \
        for (struct LLNode_##name *curr = head; curr != 0;             \
             ++size, curr = curr->next)                                \
            ;                                                          \
        return size;                                                   \
    }

#define LL_DECL_FREMOVENODE(name)                                  \
    void linked_list_removenode_##name(struct LLNode_##name *node, \
                                       struct LLNode_##name **head);

// Remove node from linked-list with given head. `node` is not de-allocated in
// any way.
#define LL_CREATE_FREMOVENODE(name)                                    \
    void linked_list_removenode_##name(struct LLNode_##name *node,     \
                                       struct LLNode_##name **head) {  \
        if (node == *head) {                                           \
            *head = node->next;                                        \
        } else {                                                       \
            struct LLNode_##name *curr = *head;                        \
            while (curr != 0 && curr->next != node) curr = curr->next; \
            curr->next = curr->next->next;                             \
        }                                                              \
    }

#endif