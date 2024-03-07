#include "registers.h"
#include "util.h"
#include <string.h>

T_i8 cpu_reg_offset_from_string(const char *string) {
    if (string[0] == 'r' && IS_DIGIT(string[1]) && !IS_DIGIT(string[2])) return (T_i8) (string[1] - '0');
    if (strcmp(string, REG_FLAG_SYM) == 0) return REG_FLAG;
    if (strcmp(string, REG_CMP_SYM) == 0) return REG_CMP;
    if (strcmp(string, REG_CCR_SYM) == 0) return REG_CCR;
    if (strcmp(string, REG_ERR_SYM) == 0) return REG_ERR;
    if (strcmp(string, REG_IP_SYM) == 0) return REG_IP;
    if (strcmp(string, REG_SP_SYM) == 0) return REG_SP;
    if (strcmp(string, REG_STACK_SIZE_SYM) == 0) return REG_STACK_SIZE;
    if (strcmp(string, REG_FP_SYM) == 0) return REG_FP;
    return -1;
}
