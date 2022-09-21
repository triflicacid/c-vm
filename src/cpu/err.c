#include "err.h"

#include <stdio.h>

#include "cpu.h"
#include "opcodes.h"

void err_print(struct CPU *cpu) {
    if (cpu->err == 0) {
        printf("No error\n");
    } else {
        printf("Error Code: %.8X\n", cpu->err);
        printf("Error Data: " WORD_T_FLAG "\n", cpu->err_data);
        switch (cpu->err) {
            case ERR_MEMOOB:
                printf(
                    "Attempted to access out-of-bounds memory address "
                    "0x%.8X\n",
                    cpu->err_data);
                break;
            case ERR_REG:
                printf("ERROR: Illegal register offset +%.2X.\n",
                       cpu->err_data);
                break;
            case ERR_UNINST:
                printf("ERROR: Unknown instruction %.4X.\n", cpu->err_data);
                break;
            default:
                break;
        }
    }
}