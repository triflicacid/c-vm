#include "instruction.h"

LL_CREATE_FINSERT(AsmInstruction, struct AsmInstruction);

LL_CREATE_FINSERTNODE(AsmInstruction);

void asm_print_instruction(struct AsmInstruction *instruct) {
    printf("Offset: +%llu; Bytes: %u; Mnemonic: \"%s\"; Opcode = %Xh",
           instruct->offset, instruct->bytes, instruct->mnemonic,
           instruct->opcode);
    // Arguments
    struct LL_NODET_NAME(AsmArgument) *a_curr = instruct->args;
    unsigned int argc = linked_list_size_AsmArgument(a_curr);
    printf("; Args: %lu\n", argc);
    while (a_curr != 0) {
        printf("\t- ");
        print_asm_arg(&(a_curr->data));
        printf("\n");
        a_curr = a_curr->next;
    }
    printf("\n");
}

void asm_print_instruction_list(struct LL_NODET_NAME(AsmInstruction) * head) {
    struct LL_NODET_NAME(AsmInstruction) *curr = head;
    while (curr != 0) {
        asm_print_instruction((&curr->data));
        curr = curr->next;
    }
}

void asm_free_instruction_list(struct LL_NODET_NAME(AsmInstruction) * head) {
    struct LL_NODET_NAME(AsmInstruction) *i_curr = head, *i_next = 0;
    while (i_curr != 0) {
        i_next = i_curr->next;
        // Free mnemonic
        free(i_curr->data.mnemonic);
        // Free arguments
        struct LL_NODET_NAME(AsmArgument) *a_curr = i_curr->data.args,
                                          *a_next = 0;
        while (a_curr != 0) {
            a_next = a_curr->next;
            free(a_curr);
            a_curr = a_next;
        }
        // Free node
        free(i_curr);
        i_curr = i_next;
    }
}