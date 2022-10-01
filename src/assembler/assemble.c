#include "assemble.h"

#include <stdlib.h>

#include "../cpu/cpu.h"
#include "../cpu/opcodes.h"
#include "err.h"

struct Assemble assemble(FILE *fp, void *buf, unsigned int buf_size,
                         unsigned int print_errors) {
    struct Assemble out = {
        .buf_offset = 0, .col = 0, .errc = ASM_ERR_NONE, .line = 0};
    char string[ASM_MAX_LINE_LENGTH];        // Line buffer
    char mnemonic[ASM_MAX_MNEMONIC_LENGTH];  // Mnemonic buffer
    char astr[ASM_MAX_LINE_LENGTH / 2];      // Argument buffer
    struct AsmArgument args[ASM_MAX_ARGS];   // Array of arguments

    unsigned int *line = &out.line, *buf_off = &out.buf_offset;
    while (1) {
        if (!fgets(string, sizeof(string), fp)) break;
        int nargs = 0, *pos = &out.col, slen = 0;
        *pos = 0;

        // Calculate string length
        for (T_u16 i = 0; string[i] != '\0' && string[i] != ';'; ++i, ++slen)
            ;

        // Eat leading whitespace
        for (; *pos < slen && IS_SEPERATOR(string[*pos]); ++(*pos))
            ;

        // Empty line?
        if (*pos == slen) {
            // printf("Line %i, length %i (empty)\n", *line, slen);
            ++(*line);
            continue;
        }

        // Get mnemonic
        int moff = 0;
        for (; *pos < slen && !IS_SEPERATOR(string[*pos]); ++(*pos), ++moff)
            mnemonic[moff] = string[*pos];
        mnemonic[moff] = '\0';

        // printf("Line %i, length %i, mnemonic '%s'\n", *line, slen, mnemonic);

        // Get arguments
        for (int i = 0; *pos < slen; ++i, ++nargs) {
            // If exceeded maximum argument count...
            if (nargs >= ASM_MAX_ARGS) {
                out.errc = ASM_ERR_GENERIC;
                if (print_errors)
                    printf(
                        "ERROR! Line %i, column %i:\nArgument count for '%s' "
                        "exceeded\n",
                        *line, *pos, mnemonic);
                return out;
            }

            // Eat whitespace
            for (; *pos < slen && IS_SEPERATOR(string[*pos]); ++(*pos))
                ;

            // Extract content up to whitespace or ","
            int spos = 0;
            for (; *pos < slen && !IS_SEPERATOR(string[*pos]); ++spos, ++(*pos))
                astr[spos] = string[*pos];
            astr[spos] = '\0';
            // printf(" - Arg: '%s'\n", astr);

            if (spos == 0)  // No arguments
                break;
            else if (astr[0] == '-' || astr[0] == '+' || IS_DIGIT(astr[0]) ||
                     IS_CHAR(astr[0])) {  // Literal/Register
                T_i8 reg_off = cpu_reg_offset_from_string(astr);
                if (reg_off == -1) {  // Parse as literal
                    T_i64 lit = str_to_int(astr, spos);
                    args[i].type = ASM_ARG_LIT;
                    args[i].data = lit;
                } else {
                    args[i].type = ASM_ARG_REG;
                    args[i].data = reg_off;
                }
            } else if (astr[0] == '[') {  // Address/Register pointer
                if (astr[spos - 1] != ']') {
                    if (print_errors)
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
                        if (print_errors)
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
                if (print_errors)
                    printf(
                        "ERROR! Line %i, column %i:\nUnknown argument format "
                        "'%s'\n",
                        *line, *pos, astr);
                out.errc = ASM_ERR_ARG;
                return out;
            }

            // printf(" -   Type: %u, data: %lli\n", args[i].type,
            // args[i].data);
        }

        int errc =
            decode_instruction(buf, buf_size, buf_off, mnemonic, args, nargs);
        if (errc == ASM_ERR_MEMORY) {
            if (print_errors)
                printf(
                    "ERROR! Line %i, column %i:\nNot enough memory - %ub limit "
                    "reached\n",
                    *line, *pos, buf_size);
            out.errc = errc;
            return out;
        } else if (errc == ASM_ERR_MNEMONIC) {
            if (print_errors)
                printf("ERROR! Line %i, column %i:\nUnknown mnemonic '%s'\n",
                       *line, *pos, mnemonic);
            out.errc = errc;
            return out;
        } else if (errc == ASM_ERR_ARGS) {
            if (print_errors)
                printf(
                    "ERROR! Line %i, column %i:\nUnknown argument(s) for "
                    "'%s'\n",
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
            RET_MEMOV(sizeof(OPCODE_T));
            BUF_WRITE(*buf_offset, OPCODE_T, OP_HALT);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "add") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_ADD_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_ADD_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR &&
                   args[2].type == ASM_ARG_ADDR) {
            WRITE_INST3(OP_ADD_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "addf32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_ADDF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_ADDF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "addf64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_ADDF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_ADDF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "and") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_AND_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_AND_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR &&
                   args[2].type == ASM_ARG_ADDR) {
            WRITE_INST3(OP_AND_REG_REG, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "and8") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_AND8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "and16") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_AND16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "and32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_AND32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "and64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_AND64_REG_LIT, T_u8, T_u64);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "cal") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_CALL_LIT, UWORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CALL_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ci8i16") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_i8_i16, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ci16i8") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_i16_i8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ci16i32") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_i16_i32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ci32i16") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_i32_i16, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ci32i64") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_i32_i64, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ci64i32") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_i64_i32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ci32f32") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_i32_f32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "cf32i32") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_f32_i32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ci64f64") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_i64_f64, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "cf64i64") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CVT_f64_i64, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "cmp") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_CMP_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_CMP_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR &&
                   args[2].type == ASM_ARG_ADDR) {
            WRITE_INST3(OP_CMP_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "cmpf32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_CMPF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_CMPF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "cmpf64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_CMPF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_CMPF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "div") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_DIV_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_DIV_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "divf32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_DIVF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_DIVF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "divf64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_DIVF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_DIVF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "inp") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_GET_CHAR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "jmp") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_JMP_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_JMP_LIT, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "jeq") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_JMP_EQ_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_JMP_EQ_LIT, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "jne") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_JMP_NEQ_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_JMP_NEQ_LIT, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "jlt") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_JMP_LT_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_JMP_LT_LIT, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "jgt") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_JMP_GT_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_JMP_GT_LIT, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "mov") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_LIT &&
            args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV_LIT_REG, WORD_T, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_MOV_LIT_MEM, WORD_T, UWORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_ADDR &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV_MEM_REG, UWORD_T, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_MOV_REG_MEM, T_u8, UWORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REGPTR &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REGPTR) {
            WRITE_INST2(OP_MOV_REG_REGPTR, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "mov8") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_ADDR &&
            args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV8_MEM_REG, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_MOV8_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REGPTR &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV8_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REGPTR) {
            WRITE_INST2(OP_MOV8_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "mov16") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_ADDR &&
            args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV16_MEM_REG, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_MOV16_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REGPTR &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV16_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REGPTR) {
            WRITE_INST2(OP_MOV16_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "mov32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_ADDR &&
            args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV32_MEM_REG, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_MOV32_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REGPTR &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV32_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REGPTR) {
            WRITE_INST2(OP_MOV32_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "mov64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_ADDR &&
            args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV64_MEM_REG, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_MOV64_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REGPTR &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MOV64_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REGPTR) {
            WRITE_INST2(OP_MOV64_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "mul") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_MUL_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MUL_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "mulf32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_MULF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MULF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "mulf64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_MULF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_MULF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "neg") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_NEG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "negf32") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_NEGF32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "negf64") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_NEGF64, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "not") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_NOT_REG, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_NOT_MEM, T_u8, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "or") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_OR_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_OR_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR &&
                   args[2].type == ASM_ARG_ADDR) {
            WRITE_INST3(OP_OR_REG_REG, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "or8") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_OR8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "or16") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_OR16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "or32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_OR32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "or64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_OR64_REG_LIT, T_u8, T_u64);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "pop") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_POP_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_POP_REGPTR, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_REGPTR) {
            WRITE_INST2(OP_POPN_REGPTR, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_POPN_MEM, T_u8, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "pop8") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_POP8_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_POP8_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "pop16") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_POP16_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_POP16_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "pop32") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_POP32_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_POP32_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "pop64") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_POP64_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_POP64_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "prc") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_LIT &&
            args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_PRINT_CHARS_MEM, T_u8, UWORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_PRINT_CHARS_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "prh") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_LIT &&
            args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_PRINT_HEX_MEM, T_u8, UWORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_PRINT_HEX_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "prs") == 0) {
        if (argc == 0) {
            BUF_WRITE(*buf_offset, OPCODE_T, OP_PSTACK);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "prr") == 0) {
        if (argc == 0) {
            BUF_WRITE(*buf_offset, OPCODE_T, OP_PREG);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "psh") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_PUSH_LIT, WORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_ADDR) {
            WRITE_INST1(OP_PUSH_MEM, UWORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR) {
            WRITE_INST2(OP_PUSHN_MEM, T_u8, UWORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_PUSH_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_PUSH_REGPTR, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_REGPTR) {
            WRITE_INST2(OP_PUSHN_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "psh8") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_PUSH8_LIT, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_ADDR) {
            WRITE_INST1(OP_PUSH8_MEM, UWORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_PUSH8_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_PUSH8_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "psh16") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_PUSH16_LIT, T_u16);
        } else if (argc == 1 && args[0].type == ASM_ARG_ADDR) {
            WRITE_INST1(OP_PUSH16_MEM, UWORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_PUSH16_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_PUSH16_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "psh32") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_PUSH32_LIT, T_u32);
        } else if (argc == 1 && args[0].type == ASM_ARG_ADDR) {
            WRITE_INST1(OP_PUSH32_MEM, UWORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_PUSH32_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_PUSH32_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "psh64") == 0) {
        if (argc == 1 && args[0].type == ASM_ARG_LIT) {
            WRITE_INST1(OP_PUSH64_LIT, T_u64);
        } else if (argc == 1 && args[0].type == ASM_ARG_ADDR) {
            WRITE_INST1(OP_PUSH64_MEM, UWORD_T);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_PUSH64_REG, T_u8);
        } else if (argc == 1 && args[0].type == ASM_ARG_REGPTR) {
            WRITE_INST1(OP_PUSH64_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "ret") == 0) {
        if (argc == 0) {
            BUF_WRITE(*buf_offset, OPCODE_T, OP_RET);
        } else if (argc == 1 && args[0].type == ASM_ARG_REG) {
            WRITE_INST1(OP_CALL_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "sar") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_ARSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_ARSHIFT_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "sll") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_LLSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_LLSHIFT_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "slr") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_LRSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_LRSHIFT_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "sub") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_SUB_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_SUB_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "subf32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_SUBF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_SUBF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "subf64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_SUBF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_SUBF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "xor") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_XOR_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && args[0].type == ASM_ARG_REG &&
                   args[1].type == ASM_ARG_REG) {
            WRITE_INST2(OP_XOR_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && args[0].type == ASM_ARG_LIT &&
                   args[1].type == ASM_ARG_ADDR &&
                   args[2].type == ASM_ARG_ADDR) {
            WRITE_INST3(OP_XOR_REG_REG, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "xor8") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_XOR8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "xor16") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_XOR16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "xor32") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_XOR32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(mnemonic, "xor64") == 0) {
        if (argc == 2 && args[0].type == ASM_ARG_REG &&
            args[1].type == ASM_ARG_LIT) {
            WRITE_INST2(OP_XOR64_REG_LIT, T_u8, T_u64);
        } else
            return ASM_ERR_ARGS;
    } else
        return ASM_ERR_MNEMONIC;
    return ASM_ERR_NONE;
}