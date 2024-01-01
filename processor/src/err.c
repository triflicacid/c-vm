#include "err.h"

#include <stdio.h>

#include "cpu.h"

void err_print(struct CPU *cpu) {
    WORD_T err = cpu->regs[REG_ERR];
    if (err == 0) {
        printf("No error\n");
    } else {
        UWORD_T data = cpu->regs[REG_FLAG];
        printf("Error Code: %.8llX\n", err);
        printf("Error Data: " WORD_T_FLAG "\n", data);
        switch (err) {
            case ERR_MEMOOB:
                printf(
                    "Attempted to access out-of-bounds memory address "
                    "0x%.8llX\n", data);
                break;
            case ERR_REG:
                printf("ERROR: Illegal register offset +%.2llX.\n", data);
                break;
            case ERR_UNINST:
                printf("ERROR: Unknown instruction %.4llX.\n", data);
                break;
            case ERR_STACK_UFLOW:
                printf("ERROR: Stack underflow\n");
                break;
            case ERR_STACK_OFLOW:
                printf(
                    "ERROR: Stack overflow - address "
                    "%.8llX is out-of-bounds (stack lower bound: %.8llX)\n",
                    data, cpu->mem_size - cpu->regs[REG_SSIZE]);
                break;
            default:
                break;
        }
    }
}