#include "instruction.h"

void asm_print_instruction(struct AsmInstruction *instruct) {
    printf("Mnemonic: \"%s\"; Opcode = %Xh; %u bytes", instruct->mnemonic,
           instruct->opcode, instruct->bytes);
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

void asm_free_instruction_chunk(struct AsmChunk *chunk) {
    struct AsmInstruction *instruct = chunk->data;
    free(instruct->mnemonic);
    struct LL_NODET_NAME(AsmArgument) *arg = instruct->args, *next = 0;
    while (arg != 0) {
        next = arg->next;
        free(arg);
        arg = next;
    }
    free(instruct);
}