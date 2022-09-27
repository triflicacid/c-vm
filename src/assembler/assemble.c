#include "assemble.h"

#include <stdlib.h>

#include "../cpu/cpu.h"
#include "../cpu/opcodes.h"
#include "err.h"

struct Assemble assemble(FILE *fp, void *buf, unsigned int buf_size) {
    struct Assemble out = {
        .buf_offset = 0, .col = 0, .errc = ASM_ERR_NONE, .line = 0};
    char string[ASM_MAX_LINE_LENGTH];        // Line buffer
    char mnemonic[ASM_MAX_MNEMONIC_LENGTH];  // Mnemonic buffer
    char astr[ASM_MAX_LINE_LENGTH / 2];      // Argument buffer
    struct AsmArgument args[ASM_MAX_ARGS];   // Array of arguments

    unsigned int *line = &out.line, *buf_off = &out.buf_offset;
    while (1) {
        if (!fgets(string, sizeof(string), fp)) break;
        // printf("Line %u: %s\n", line, string);
        int nargs = 0, *pos = &out.col, slen = strlen(string);
        *pos = 0;

        // Eat leading whitespace
        for (; *pos < slen && IS_WHITESPACE(string[*pos]); ++(*pos))
            ;

        // Get mnemonic
        int moff = 0;
        for (; *pos < slen && !IS_WHITESPACE(string[*pos]); ++(*pos), ++moff)
            mnemonic[moff] = string[*pos];
        mnemonic[moff] = '\0';

        printf("Line %i, mnemonic '%s'\n", *line, mnemonic);

        // Get arguments
        for (int i = 0; *pos < slen; ++i, ++nargs) {
            // Eat whitespace
            for (; *pos < slen && IS_WHITESPACE(string[*pos]); ++(*pos))
                ;
            int spos = 0;
            for (; *pos < slen && !IS_WHITESPACE(string[*pos]);
                 ++spos, ++(*pos))
                astr[spos] = string[*pos];
            astr[spos] = '\0';
            printf(" - Arg: '%s'\n", astr);

            if (spos == 0)  // No arguments
                break;
            else if (IS_CHAR(astr[0])) {  // Register?
                T_i8 reg_off = cpu_reg_offset_from_string(astr);
                if (reg_off == -1) {  // Unknown register
                    printf(
                        "ERROR! Line %i, column %i:\nUnknown register '%s'\n",
                        *line, *pos, astr);
                    out.errc = ASM_ERR_REG;
                    return out;
                }
                args[i].type = ASM_ARG_REG;
                args[i].data = reg_off;
            } else if (astr[0] == '-' || astr[0] == '+' ||
                       IS_DIGIT(astr[0])) {  // Literal
                T_i64 lit = str_to_int(astr, spos);
                args[i].type = ASM_ARG_LIT;
                args[i].data = lit;
            } else if (astr[0] == '[') {  // Address/Register pointer
                if (astr[spos - 1] != ']') {
                    printf(
                        "ERROR! Line %i, column %i:\nExpected ']' after "
                        "address expression: "
                        "'%s' <-- ]\n",
                        *line, *pos, astr);
                    out.errc = ASM_ERR_ADDR;
                    return out;
                }
                if (IS_CHAR(astr[1])) {     // Register pointer?
                    astr[spos - 1] = '\0';  // Remove ']'
                    T_i8 reg_off = cpu_reg_offset_from_string(astr + 1);
                    if (reg_off == -1) {  // Unknown register
                        printf(
                            "ERROR! Line %i, column %i:\nUnknown register "
                            "pointer '[%s]'\n",
                            *line, *pos, astr + 1);
                        out.errc = ASM_ERR_REG;
                        return out;
                    }
                    args[i].type = ASM_ARG_REGPTR;
                    args[i].data = reg_off;
                } else {
                    T_i64 addr = str_to_int(astr + 1, spos - 2);
                    args[i].type = ASM_ARG_ADDR;
                    args[i].data = addr;
                }
            } else {  // Unknown argument form
                printf(
                    "ERROR! Line %i, column %i:\nUnknown argument format "
                    "'%s'\n",
                    *line, *pos, astr);
                out.errc = ASM_ERR_ARG;
                return out;
            }

            printf(" -   Type: %u, data: %lli\n", args[i].type, args[i].data);
        }

        int errc =
            decode_instruction(buf, buf_size, buf_off, mnemonic, args, nargs);
        if (errc == ASM_ERR_MEMORY) {
            printf(
                "ERROR! Line %i, column %i:\nNot enough memory - %ub limit "
                "reached\n",
                *line, *pos, buf_size);
            out.errc = errc;
            return out;
        } else if (errc == ASM_ERR_MNEMONIC) {
            printf("ERROR! Line %i, column %i:\nUnknown mnemonic '%s'\n", *line,
                   *pos, mnemonic);
            out.errc = errc;
            return out;
        } else if (errc == ASM_ERR_OPERAND) {
            printf("ERROR! Line %i, column %i:\nUnknown operand(s) for '%s'\n",
                   *line, *pos, mnemonic);
            out.errc = errc;
            return out;
        }

        (*line)++;
    }

    return out;
}

int decode_instruction(void *buf, unsigned int buf_size,
                       unsigned int *buf_offset, const char *mnemonic,
                       struct AsmArgument *args, unsigned int argc) {
    if (strcmp(mnemonic, "hlt") == 0) {
        if (argc == 0) {
            BUF_WRITE(*buf_offset, OPCODE_T, OP_HALT);
        } else
            return ASM_ERR_OPERAND;
    } else if (strcmp(mnemonic, "mov") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_LIT &&
            args[1].type == ASM_ARG_REG) {
            RET_MEMOV(sizeof(OPCODE_T) + sizeof(WORD_T) + sizeof(T_u8));
            BUF_WRITE(*buf_offset, OPCODE_T, OP_MOV_LIT_REG);
            BUF_WRITE(*buf_offset, WORD_T, args[0].data);
            BUF_WRITE(*buf_offset, T_u8, args[1].data);
        } else if (argc == 2 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR) {
            RET_MEMOV(sizeof(OPCODE_T) + sizeof(WORD_T) + sizeof(UWORD_T));
            BUF_WRITE(*buf_offset, OPCODE_T, OP_MOV_LIT_REG);
            BUF_WRITE(*buf_offset, WORD_T, args[0].data);
            BUF_WRITE(*buf_offset, UWORD_T, args[1].data);
        } else if (argc == 2 && args[0].type == ASM_ARG_ADDR &&
                   args[1].type == ASM_ARG_REG) {
            RET_MEMOV(sizeof(OPCODE_T) + sizeof(UWORD_T) + sizeof(T_u8));
            BUF_WRITE(*buf_offset, OPCODE_T, OP_MOV_MEM_REG);
            BUF_WRITE(*buf_offset, UWORD_T, args[0].data);
            BUF_WRITE(*buf_offset, T_u8, args[1].data);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_ADDR) {
            RET_MEMOV(sizeof(OPCODE_T) + sizeof(T_u8) + sizeof(UWORD_T));
            BUF_WRITE(*buf_offset, OPCODE_T, OP_MOV_REG_MEM);
            BUF_WRITE(*buf_offset, T_u8, args[0].data);
            BUF_WRITE(*buf_offset, UWORD_T, args[1].data);
        } else if (argc == 2 && args[0].type == ASM_ARG_REGPTR &&
                   args[1].type == ASM_ARG_REG) {
            RET_MEMOV(sizeof(OPCODE_T) + sizeof(T_u8) + sizeof(T_u8));
            BUF_WRITE(*buf_offset, OPCODE_T, OP_MOV_REGPTR_REG);
            BUF_WRITE(*buf_offset, T_u8, args[0].data);
            BUF_WRITE(*buf_offset, T_u8, args[1].data);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REGPTR) {
            RET_MEMOV(sizeof(OPCODE_T) + sizeof(T_u8) + sizeof(T_u8));
            BUF_WRITE(*buf_offset, OPCODE_T, OP_MOV_REG_REGPTR);
            BUF_WRITE(*buf_offset, T_u8, args[0].data);
            BUF_WRITE(*buf_offset, T_u8, args[1].data);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            RET_MEMOV(sizeof(OPCODE_T) + sizeof(T_u8) + sizeof(T_u8));
            BUF_WRITE(*buf_offset, OPCODE_T, OP_MOV_REG_REG);
            BUF_WRITE(*buf_offset, T_u8, args[0].data);
            BUF_WRITE(*buf_offset, T_u8, args[1].data);
        } else
            return ASM_ERR_OPERAND;
    } else
        return ASM_ERR_MNEMONIC;
    if (*buf_offset >= buf_size) return ASM_ERR_MEMORY;
    return ASM_ERR_NONE;
}