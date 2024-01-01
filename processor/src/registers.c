#include "registers.h"
#include <string.h>

int cpu_reg_print(struct CPU *cpu) {
    for (int i = 0; i < REG_COUNT; ++i) {
        char str[10];
        switch (i) {
            case REG_ERR:
                sprintf(str, REG_ERR_SYM);
                break;
            case REG_FLAG:
                sprintf(str, REG_FLAG_SYM);
                break;
            case REG_CCR:
                sprintf(str, REG_CCR_SYM);
                break;
            case REG_IP:
                sprintf(str, REG_IP_SYM);
                break;
            case REG_SP:
                sprintf(str, REG_SP_SYM);
                break;
            case REG_FP:
                sprintf(str, REG_FP_SYM);
                break;
            case REG_SSIZE:
                sprintf(str, REG_SSIZE_SYM);
                break;
            default:
                sprintf(str, "r%i", i);
                break;
        }
        printf("%s | ", str);
        if (IS_BIG_ENDIAN)
            for (T_u8 j = 0; j < sizeof(WORD_T); --j)
                printf("%.2x", *((T_u8*)cpu->regs + i * sizeof(WORD_T) + j));
        else
            for (T_u8 j = sizeof(WORD_T); j > 0; --j)
                printf("%.2x",
                       *((T_u8*)cpu->regs + i * sizeof(WORD_T) + (j - 1)));
        printf("\n");
    }
    return ERR_NONE;
}

void cpu_reg_write(struct CPU* cpu, unsigned int reg_offset, WORD_T value) {
    cpu->regs[reg_offset] = value;
}

WORD_T cpu_reg_read(struct CPU* cpu, unsigned int reg_offset) {
    return cpu->regs[reg_offset];
}

T_i8 cpu_reg_offset_from_string(const char *string) {
    if (string[0] == 'r') return string[1] - '0';
    if (strcmp(string, REG_FLAG_SYM) == 0) return REG_FLAG;
    if (strcmp(string, REG_CMP_SYM) == 0) return REG_CMP;
    if (strcmp(string, REG_CCR_SYM) == 0) return REG_CCR;
    if (strcmp(string, REG_ERR_SYM) == 0) return REG_ERR;
    if (strcmp(string, REG_IP_SYM) == 0) return REG_IP;
    if (strcmp(string, REG_SP_SYM) == 0) return REG_SP;
    if (strcmp(string, REG_SSIZE_SYM) == 0) return REG_SSIZE;
    if (strcmp(string, REG_FP_SYM) == 0) return REG_FP;
    return -1;
}