#include "args.h"

#include "../cpu/bit-ops.h"

LL_CREATE_FINSERT(AsmArgument, struct AsmArgument);

LL_CREATE_FINSERTNODE(AsmArgument);

LL_CREATE_FSIZE(AsmArgument);

int print_asm_arg(struct AsmArgument *arg) {
    switch (arg->type) {
        case ASM_ARG_LIT:
            // printf("literal '%lf'", *(double *)&(arg->data));
            printf("literal '%llu' {", arg->data);
            print_bytes(&(arg->data), 8);
            printf("\b}");
            break;
        case ASM_ARG_ADDR:
            printf("address '%llu'", arg->data);
            break;
        case ASM_ARG_REG:
            printf("register '%llu'", arg->data);
            break;
        case ASM_ARG_REGPTR:
            printf("register pointer '%llu'", arg->data);
            break;
        case ASM_ARG_LABEL:
            printf("label \"%s\"", (char *)arg->data);
            break;
        default:
            printf("(unknown)");
            return 1;
    }
    return 0;
}