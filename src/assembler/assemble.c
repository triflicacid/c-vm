#include "assemble.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cpu/cpu.h"
#include "../cpu/opcodes.h"
#include "args.h"
#include "err.h"
#include "instruction.h"
#include "labels.h"
#include "line.h"
#include "symbol.h"

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

struct Assemble assemble(FILE *fp, void *buf, unsigned int buf_size,
                         unsigned int print_errors, int debug) {
    struct Assemble out = {
        .buf_offset = 0, .col = 0, .errc = ASM_ERR_NONE, .line = 0};
    struct LL_NODET_NAME(AsmLine) *line_llhead = 0;    // Linked list for lines
    char string[ASM_MAX_LINE_LENGTH];                  // Line buffer
    struct LL_NODET_NAME(AsmLabel) *label_llhead = 0;  // Linked list for labels
    struct LL_NODET_NAME(AsmInstruction) *instruction_llhead =
        0;  // Linked list for instructions
    struct LL_NODET_NAME(AsmSymbol) *symbol_llhead =
        0;  // Linked list for symbols

    //#region READ LINES
    out.line = 0;
    while (1) {
        if (!fgets(string, sizeof(string), fp)) break;
        unsigned int len = 0, i;
        // Calculate string length
        for (i = 0; string[i] != '\0' && string[i] != ';'; ++i, ++len)
            ;
        i = 0;
        // Eat leading whitespace
        for (; i < len && IS_WHITESPACE(string[i]); ++i)
            ;
        // Create node?
        if (i < len) {
            char *str = extract_string(string, 0, len);

            struct LL_NODET_NAME(AsmLine) *node =
                malloc(sizeof(struct LL_NODET_NAME(AsmLine)));
            node->data.n = out.line;
            node->data.len = len;
            node->data.str = str;
            node->data.done = 0;
            linked_list_insertnode_AsmLine(node, &line_llhead, -1);
        }
        ++out.line;
    }
    //#endregion

    //#region HANDLE PRE-PROCESSOR DIRECTIVES
    if (debug) printf("=== Pre-Processing ===\n");
    struct LL_NODET_NAME(AsmLine) *cline = line_llhead;
    while (cline != 0) {
        unsigned int idx = 0;
        unsigned int slen = cline->data.len;
        char *string = cline->data.str;

        // Eat leading ws
        while (idx < slen && IS_WHITESPACE(string[idx])) ++idx;

        // Replace constants?
        unsigned int idx2 = idx;
        struct LL_NODET_NAME(AsmSymbol) *csym = symbol_llhead;
        while (csym != 0) {
            unsigned int len = strlen(csym->data.name), vlen = strlen(csym->data.value);
            while (idx2 < slen) {
                // Eas ws
                while (idx2 < slen && IS_WHITESPACE(string[idx2])) ++idx2;
                if (idx2 == slen) break;

                unsigned int match = 1, idxn = 0;
                while (idxn < len && match &&
                       !IS_WHITESPACE(string[idx2 + idxn])) {
                    if (csym->data.name[idxn] != string[idx2 + idxn])
                        match = 0;
                    else
                        ++idxn;
                }
                if (match) {
                    if (debug) printf("FOUND match for symbol \"%s\" at %u:%u\n",
                           csym->data.name, cline->data.n, idx2);
                    // TODO: Replace name with value
                    unsigned int new_len = slen - len + vlen;
                    char *new_str = malloc(new_len);
                    memcpy(new_str, string, idx2);
                    memcpy(new_str + idx2, csym->data.value, vlen);
                    memcpy(new_str + idx2 + vlen, string + idx2 + len, slen - idx2);
                    free(string);
                    cline->data.str = string = new_str;
                    cline->data.len = slen = new_len;
                }
                // Skip to next ws
                while (idx2 < slen && !IS_WHITESPACE(string[idx2])) ++idx2;
            }
            csym = csym->next;
        }

        if (string[idx] == '%') {               // Directive
            unsigned int didx = idx, dlen = 0;  // Directive index/length
            while (idx < slen && !IS_WHITESPACE(string[idx])) ++idx, ++dlen;

            char *directive = extract_string(string, didx + 1, dlen - 1);
            if (strcmp(directive, "define") == 0) {
                while (idx < slen && IS_WHITESPACE(string[idx])) ++idx;
                // Extract symbol name
                unsigned int name_i = idx, name_len = 0;
                while (idx < slen && !IS_WHITESPACE(string[idx]))
                    ++idx, ++name_len;
                char *name = extract_string(string, name_i, name_len);
                // Extract value (read to EOL)
                ++idx;
                unsigned int vlen = slen - idx - 1;  // Value length
                while (string[vlen - 1] == '\r' || string[vlen - 1] == '\n')
                    --vlen;  // Remove newline chars
                char *value = extract_string(string, idx, vlen);
                if (debug) printf("DEFINE \"%s\" TO BE \"%s\"\n", name, value);
                idx = slen;

                struct LL_NODET_NAME(AsmSymbol) *node =
                    malloc(sizeof(struct LL_NODET_NAME(AsmSymbol)));
                node->data.name = name;
                node->data.value = value;
                linked_list_insertnode_AsmSymbol(node, &symbol_llhead, -1);
            } else {
                if (print_errors)
                    printf(
                        "ERROR! Line %i, column %i:\nUnknown directive "
                        "%%%s\n",
                        cline->data.n, idx, directive);
                free(directive);
                out.errc = ASM_ERR_DIRECTIVE;
                goto assemble_exit;
            }
            free(directive);
            cline->data.done = 1;
            cline = cline->next;
            continue;
        }
        cline = cline->next;
    }
    //#endregion

    //#region PARSE AST
    if (debug) printf("=== Parsing ===\n");
    unsigned int offset = 0;
    cline = line_llhead;
    while (cline != 0) {
        // Parsed already?
        if (cline->data.done != 0) {
            cline = cline->next;
            continue;
        }

        int *pos = &out.col;
        const unsigned int slen = cline->data.len, line = cline->data.n;
        const char *string = cline->data.str;
        *pos = 0;

        // Eat leading whitespace
        for (; *pos < slen && IS_WHITESPACE(string[*pos]); ++(*pos))
            ;

        // Empty line?
        if (*pos == slen) {
            if (debug) printf("Line %i, length %i (empty)\n", line, slen);
            continue;
        }

        // Get first item
        int mptr = *pos, mlen = 0;
        for (; *pos < slen && !IS_WHITESPACE(string[*pos]); ++(*pos), ++mlen)
            ;

        // LABEL
        if (string[mptr + mlen - 1] == ':') {
            char *lbl = extract_string(string, mptr, mlen - 1);  // Label string
            if (debug) {
                printf("Label \"%s\" at offset +%u\n", lbl, offset);
            }
            // Does label already exist?
            struct AsmLabel *label =
                linked_list_find_AsmLabel(label_llhead, lbl);
            if (label == 0) {  // Create new label
                struct LL_NODET_NAME(AsmLabel) *node =
                    malloc(sizeof(struct LL_NODET_NAME(AsmLabel)));
                node->data.ptr = lbl;
                node->data.len = mlen - 1;
                node->data.addr = offset;
                linked_list_insertnode_AsmLabel(node, &label_llhead, -1);

                // Replace past references to this label
                struct LL_NODET_NAME(AsmInstruction) *instruct =
                    instruction_llhead;
                while (instruct != 0) {
                    struct LL_NODET_NAME(AsmArgument) *arg =
                        instruct->data.args;
                    int i = 0;
                    while (arg != 0) {
                        if (arg->data.type == ASM_ARG_LABEL &&
                            strcmp(lbl, (char *)arg->data.data) == 0) {
                            if (debug)
                                printf(
                                    "Mnemonic \"%s\"/Opcode %u, arg %i: "
                                    "label \"%s\" "
                                    "-> addr [%llu]\n",
                                    instruct->data.mnemonic,
                                    instruct->data.opcode, i, lbl, offset);
                            arg->data.type = ASM_ARG_ADDR;
                            arg->data.data = offset;
                        }
                        ++i;
                        arg = arg->next;
                    }
                    instruct = instruct->next;
                }
            } else {  // Update label
                label->addr = offset;
                free(lbl);
            }
            int size = 0;
            for (; *pos < slen && IS_WHITESPACE(string[*pos]); ++(*pos))
                ;
            if (*pos == slen) {  // End of line?
                cline = cline->next;
                continue;
            }
            for (mptr = *pos, mlen = 0;
                 *pos < slen && !IS_WHITESPACE(string[*pos]); ++(*pos), ++mlen)
                ;
        }

        // Create data
        struct LL_NODET_NAME(AsmInstruction) *instruct_node =
            malloc(sizeof(struct LL_NODET_NAME(AsmInstruction)));
        struct AsmInstruction *instruct = &(instruct_node->data);
        instruct->offset = offset;
        instruct->bytes = 0;
        instruct->args = 0;
        linked_list_insertnode_AsmInstruction(instruct_node,
                                              &instruction_llhead, -1);

        // Transfer over mnemonic
        instruct->mnemonic = malloc(mlen + 1);
        memcpy(instruct->mnemonic, string + mptr, mlen);
        *((char *)instruct->mnemonic + mlen) = '\0';

        if (debug) {
            printf("Line %i, length %i, mnemonic '%s'\n", line, slen,
                   instruct->mnemonic);
        }

        // Get arguments
        while (*pos < slen) {
            // Eat whitespace
            for (; *pos < slen && IS_WHITESPACE(string[*pos]); ++(*pos))
                ;
            if (*pos == slen) break;

            if (string[*pos] == '\'') {  // CHARACTER LITERAL
                int j = 0, k = 0;
                char data[sizeof(UWORD_T)] = {0};
                while (j < sizeof(data) && *pos < slen &&
                       string[*pos] == '\'') {
                    if (string[*pos + 1] == '\\') {  // Escape sequence
                        char *ptr = (char *)string + *pos + 2;
                        long long val = decode_escape_seq(&ptr);
                        data[j] = (char)val;
                        *pos = ptr - string + 1;
                    } else {
                        if (string[*pos + 2] == '\'') {
                            data[j] = string[*pos + 1];
                            *pos += 3;
                        } else {
                            if (print_errors) {
                                char astr[] = {string[*pos], string[*pos + 1],
                                               '\0'};
                                printf(
                                    "ERROR! Line %i, column %i:\nExpected ' "
                                    "after character expression "
                                    "%s <-- '\n",
                                    line, *pos, astr);
                            }
                            out.errc = ASM_ERR_GENERIC;
                            goto assemble_exit;
                        }
                    }
                    j++;
                    CONSUME_WHITESPACE(string, *pos);
                }

                struct LL_NODET_NAME(AsmArgument) *node =
                    malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                node->data.type = ASM_ARG_LIT;
                node->data.data = bytes_to_int(data, j);
                linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
            } else if (string[*pos] == '\"') {  // STRING LITERAL
                unsigned short j = 0;
                char data[sizeof(UWORD_T)];
                ++(*pos);
                while (j < sizeof(data) && *pos < slen) {
                    if (string[*pos] == '\"') {
                        ++(*pos);
                        break;
                    }
                    if (string[*pos] == '\\') {  // Escape sequence
                        char *ptr = (char *)string + *pos + 1;
                        long long val = decode_escape_seq(&ptr);
                        data[j] = (char)val;
                        *pos = ptr - string + 1;
                    } else {
                        data[j] = string[*pos];
                        ++(*pos);
                    }
                    j++;
                }

                struct LL_NODET_NAME(AsmArgument) *node =
                    malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                node->data.type = ASM_ARG_LIT;
                node->data.data = bytes_to_int(data, j + 1);
                linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
            } else if (string[*pos] == '[') {  // Address/Register pointer
                unsigned int len = 0;
                while (*pos + len < slen && !IS_WHITESPACE(string[*pos + len]))
                    ++len;
                if (string[*pos + len - 1] != ']') {
                    if (print_errors) {
                        char *astr = extract_string(string, *pos, len);
                        printf(
                            "ERROR! Line %i, column %i:\nExpected ']' after "
                            "address expression: '%s' <-- ]\n",
                            line, *pos, astr);
                        free(astr);
                    }
                    out.errc = ASM_ERR_ADDR;
                    goto assemble_exit;
                }
                if (IS_CHAR(string[*pos + 1])) {  // Register pointer?
                    T_i8 reg_off =
                        cpu_reg_offset_from_string((char *)string + *pos + 1);
                    if (reg_off == -1) {  // Unknown register
                        if (print_errors) {
                            char *astr =
                                extract_string(string, *pos + 1, len - 2);
                            printf(
                                "ERROR! Line %i, column %i:\nUnknown register "
                                "pointer '[%s]'\n",
                                line, *pos, astr);
                            free(astr);
                        }
                        out.errc = ASM_ERR_REG;
                        goto assemble_exit;
                    }

                    struct LL_NODET_NAME(AsmArgument) *node =
                        malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                    node->data.type = ASM_ARG_REGPTR;
                    node->data.data = reg_off;
                    linked_list_insertnode_AsmArgument(node, &(instruct->args),
                                                       -1);
                } else {
                    char *astr = extract_string(string, *pos + 1, len - 2);
                    int radix = get_radix(astr[len - 3]);
                    unsigned long long addr =
                        base_to_10(astr, radix == -1 ? 10 : radix);
                    free(astr);

                    struct LL_NODET_NAME(AsmArgument) *node =
                        malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                    node->data.type = ASM_ARG_ADDR;
                    node->data.data = addr;
                    linked_list_insertnode_AsmArgument(node, &(instruct->args),
                                                       -1);
                }
                *pos += len;
            } else if (string[*pos] == '-' || string[*pos] == '+' ||
                       IS_DIGIT(string[*pos]) ||
                       IS_CHAR(string[*pos])) {  // Literal/Register
                unsigned int sublen = 0;
                while (*pos + sublen < slen &&
                       !IS_SEPERATOR(string[*pos + sublen]) &&
                       !IS_WHITESPACE(string[*pos + sublen]))
                    ++sublen;
                char *sub = extract_string(string, *pos, sublen);
                // Check if register
                T_i8 reg_off = cpu_reg_offset_from_string(sub);
                if (reg_off == -1) {  // Is it a register?
                    int radix = get_radix(sub[sublen - 1]);
                    unsigned int litend =
                        scan_number(sub, radix == -1 ? 10 : radix);
                    int has_dp = 0;
                    for (int j = 0; j < sublen && !IS_WHITESPACE(sub[j]); ++j) {
                        if (sub[j] == '.') {
                            has_dp = 1;
                            break;
                        }
                    }
                    if (litend == sublen ||
                        (radix != -1 && litend == sublen - 1)) {  // Is literal!
                        struct LL_NODET_NAME(AsmArgument) *node =
                            malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                        node->data.type = ASM_ARG_LIT;
                        if (has_dp) {  // Float
                            double lit =
                                fbase_to_10(sub, radix == -1 ? 10 : radix);
                            node->data.data = *(unsigned long long *)&lit;
                        } else {  // Integer
                            unsigned long long lit =
                                base_to_10(sub, radix == -1 ? 10 : radix);
                            node->data.data = lit;
                        }
                        linked_list_insertnode_AsmArgument(
                            node, &(instruct->args), -1);
                    } else {  // Label
                        // Exists?
                        struct AsmLabel *lbl =
                            linked_list_find_AsmLabel(label_llhead, sub);
                        struct LL_NODET_NAME(AsmArgument) *node =
                            malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                        if (lbl == 0) {
                            node->data.type = ASM_ARG_LABEL;
                            node->data.data = (unsigned long long)sub;
                        } else {
                            node->data.type = ASM_ARG_ADDR;
                            node->data.data = lbl->addr;
                        }
                        linked_list_insertnode_AsmArgument(
                            node, &(instruct->args), -1);
                    }
                } else {
                    struct LL_NODET_NAME(AsmArgument) *node =
                        malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                    node->data.type = ASM_ARG_REG;
                    node->data.data = reg_off;
                    linked_list_insertnode_AsmArgument(node, &(instruct->args),
                                                       -1);
                }
                *pos += sublen;
            } else {  // Unknown argument form
                if (print_errors) {
                    unsigned int len = 0;
                    while (!IS_WHITESPACE(string[*pos + len])) ++len;
                    char *astr = extract_string(string, *pos, len);
                    printf(
                        "ERROR! Line %i, column %i:\nUnknown argument format "
                        "'%s'\n",
                        line, *pos, astr);
                    free(astr);
                }
                out.errc = ASM_ERR_ARG;
                goto assemble_exit;
            }

            // End of argument
            CONSUME_WHITESPACE(string, *pos);
            if (*pos == slen) {
                break;
            } else if (string[*pos] == ',') {
                ++(*pos);
            } else {
                if (print_errors)
                    printf(
                        "ERROR! Line %i, column %i:\nExpected comma, found "
                        "'%c'\n",
                        line, *pos, string[*pos]);
                out.errc = ASM_ERR_GENERIC;
                goto assemble_exit;
            }
        }

        if (debug) asm_print_instruction(instruct);

        int errc = decode_instruction(instruct);
        if (errc == ASM_ERR_MNEMONIC) {
            if (print_errors)
                printf("ERROR! Line %i, column %i:\nUnknown mnemonic '%s'\n",
                       line, *pos, instruct->mnemonic);
            out.errc = errc;
            goto assemble_exit;
        } else if (errc == ASM_ERR_ARGS) {
            if (print_errors) {
                unsigned int argc =
                    linked_list_size_AsmArgument(instruct->args);
                printf(
                    "ERROR! Line %i, column %i:\nUnknown argument(s) for "
                    "\"%s\": [%u] ",
                    line, *pos, instruct->mnemonic, argc);
                struct LL_NODET_NAME(AsmArgument) *curr = instruct->args;
                for (unsigned int i = 0; curr != 0; ++i, curr = curr->next) {
                    print_asm_arg(&(curr->data));
                    if (i < argc - 1) printf("; ");
                }
                printf("\n");
            }

            out.errc = errc;
            goto assemble_exit;
        } else if (offset + instruct->bytes > buf_size) {
            if (print_errors)
                printf(
                    "ERROR! Line %i, column %i:\nNot enough memory - %ub "
                    "limit reached\n",
                    line, *pos, offset);
            out.errc = ASM_ERR_MEMORY;
            goto assemble_exit;
        }

        offset += instruct->bytes;
        cline = cline->next;
    }
    //#endregion

    // Print AST?
    if (debug) {
        printf("=== Instruction AST ===\n");
        asm_print_instruction_list(instruction_llhead);
        printf("=== Labels ===\n");
        linked_list_print_AsmLabel(label_llhead);
        printf("=== Symbols ===\n");
        linked_list_print_AsmSymbol(symbol_llhead);
    }

    // Check for labels (should be none).
    struct LL_NODET_NAME(AsmInstruction) *instruct = instruction_llhead;
    while (instruct != 0) {
        struct LL_NODET_NAME(AsmArgument) *arg = instruct->data.args;
        int i = 0;
        while (arg != 0) {
            if (arg->data.type == ASM_ARG_LABEL) {
                if (print_errors)
                    printf(
                        "ERROR! Instruction \"%s\" (+%u): reference to "
                        "undefined "
                        "label \"%s\"\n",
                        instruct->data.mnemonic, instruct->data.offset,
                        arg->data.data);
                out.errc = ASM_ERR_LABEL;
                goto assemble_exit;
            }
            ++i;
            arg = arg->next;
        }
        instruct = instruct->next;
    }

    //#region COMPILE AST
    // Instructions
    unsigned int *buf_off = &out.buf_offset;
    instruct = instruction_llhead;
    while (instruct != 0) {
        int errc = write_instruction(buf, &(instruct->data));
        if (errc != ASM_ERR_NONE) {
            if (print_errors)
                printf("ERROR! Unknown opcode whilst decoding: %llu\n",
                       instruct->data.opcode);
            out.errc = errc;
            goto assemble_exit;
        }
        SET_IF_LARGER(*buf_off, instruct->data.offset + instruct->data.bytes);
        instruct = instruct->next;
    }
    //#endregion

    // Release un-needed memory, and exit
assemble_exit:
    linked_list_destroy_AsmLine(&line_llhead);
    asm_free_instruction_list(instruction_llhead);
    linked_list_destroy_AsmLabel(&label_llhead);
    linked_list_destroy_AsmSymbol(&symbol_llhead);

    return out;
}

int decode_instruction(struct AsmInstruction *instruct) {
    unsigned int argc = linked_list_size_AsmArgument(instruct->args);
    if (strcmp(instruct->mnemonic, "hlt") == 0) {
        if (argc == 0) {
            instruct->opcode = OP_HALT;
            instruct->bytes = sizeof(OPCODE_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "nop") == 0) {
        if (argc == 0) {
            instruct->opcode = OP_NOP;
            instruct->bytes = sizeof(OPCODE_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "add") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_ADD_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_ADD_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR &&
                   instruct->args->next->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST3(OP_ADD_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "addf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_ADDF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_ADDF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "addf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_ADDF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_ADDF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_AND_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR &&
                   instruct->args->next->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST3(OP_AND_REG_REG, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and8") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and16") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND64_REG_LIT, T_u8, T_u64);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "cal") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_LIT) {
            DECODE_INST1(OP_CALL_LIT, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CALL_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ci8i16") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_i8_i16, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ci16i8") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_i16_i8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ci16i32") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_i16_i32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ci32i16") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_i32_i16, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ci32i64") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_i32_i64, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ci64i32") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_i64_i32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ci32f32") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_i32_f32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "cf32i32") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_f32_i32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ci64f64") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_i64_f64, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "cf64i64") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_CVT_f64_i64, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "cmp") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_CMP_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_CMP_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR &&
                   instruct->args->next->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST3(OP_CMP_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "cmpf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_CMPF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_CMPF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "cmpf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_CMPF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_CMPF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "div") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_DIV_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_DIV_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "divf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_DIVF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_DIVF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "divf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_DIVF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_DIVF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "inp") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_GET_CHAR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jmp") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_REG, T_u8);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jeq") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_EQ_REG, T_u8);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_EQ_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jne") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_NEQ_REG, T_u8);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_NEQ_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jlt") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_LT_REG, T_u8);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_LT_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jgt") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_GT_REG, T_u8);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_GT_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
            instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV_LIT_REG, WORD_T, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_MOV_LIT_MEM, WORD_T, UWORD_T);
        } else if (argc == 2 &&
                   (instruct->args->data.type == ASM_ARG_ADDR ||
                    instruct->args->data.type == ASM_ARG_LABEL) &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV_MEM_REG, UWORD_T, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_MOV_REG_MEM, T_u8, UWORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV_REG_REGPTR, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov8") == 0) {
        if (argc == 2 &&
            (instruct->args->data.type == ASM_ARG_ADDR ||
             instruct->args->data.type == ASM_ARG_LABEL) &&
            instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV8_MEM_REG, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_MOV8_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV8_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV8_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov16") == 0) {
        if (argc == 2 &&
            (instruct->args->data.type == ASM_ARG_ADDR ||
             instruct->args->data.type == ASM_ARG_LABEL) &&
            instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV16_MEM_REG, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_MOV16_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV16_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV16_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov32") == 0) {
        if (argc == 2 &&
            (instruct->args->data.type == ASM_ARG_ADDR ||
             instruct->args->data.type == ASM_ARG_LABEL) &&
            instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV32_MEM_REG, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_MOV32_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV32_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV32_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov64") == 0) {
        if (argc == 2 &&
            (instruct->args->data.type == ASM_ARG_ADDR ||
             instruct->args->data.type == ASM_ARG_LABEL) &&
            instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV64_MEM_REG, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_MOV64_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV64_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV64_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mul") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_MUL_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MUL_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mulf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_MULF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MULF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mulf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_MULF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MULF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "neg") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_NEG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "negf32") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_NEGF32, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "negf64") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_NEGF64, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "not") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_NOT_REG, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_NOT_MEM, T_u8, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_OR_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR &&
                   instruct->args->next->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST3(OP_OR_REG_REG, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or8") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or16") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR64_REG_LIT, T_u8, T_u64);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "pop") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_POP_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_POP_REGPTR, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_POPN_REGPTR, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_POPN_MEM, T_u8, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "pop8") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_POP8_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_POP8_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "pop16") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_POP16_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_POP16_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "pop32") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_POP32_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_POP32_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "pop64") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_POP64_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_POP64_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "prc") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
            instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_PRINT_CHARS_MEM, T_u8, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PRINT_CHARS_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "prd") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PRINT_DBL_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "prh") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
            instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_PRINT_HEX_MEM, T_u8, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PRINT_HEX_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "pri") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PRINT_INT_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "pru") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PRINT_UINT_REG, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "prs") == 0) {
        if (argc == 0) {
            instruct->opcode = OP_PSTACK;
            instruct->bytes = sizeof(OPCODE_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "prr") == 0) {
        if (argc == 0) {
            instruct->opcode = OP_PREG;
            instruct->bytes = sizeof(OPCODE_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "psh") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_LIT) {
            DECODE_INST1(OP_PUSH_LIT, WORD_T);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_PUSH_MEM, UWORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST2(OP_PUSHN_MEM, T_u8, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PUSH_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_PUSH_REGPTR, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_PUSHN_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "psh8") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_LIT) {
            DECODE_INST1(OP_PUSH8_LIT, T_u8);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_PUSH8_MEM, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PUSH8_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_PUSH8_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "psh16") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_LIT) {
            DECODE_INST1(OP_PUSH16_LIT, T_u16);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_PUSH16_MEM, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PUSH16_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_PUSH16_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "psh32") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_LIT) {
            DECODE_INST1(OP_PUSH32_LIT, T_u32);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_PUSH32_MEM, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PUSH32_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_PUSH32_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "psh64") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_LIT) {
            DECODE_INST1(OP_PUSH64_LIT, T_u64);
        } else if (argc == 1 && (instruct->args->data.type == ASM_ARG_ADDR ||
                                 instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_PUSH64_MEM, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PUSH64_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_PUSH64_REGPTR, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "ret") == 0) {
        if (argc == 0) {
            instruct->opcode = OP_RET;
            instruct->bytes = sizeof(OPCODE_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "sar") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_ARSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_ARSHIFT_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "sll") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_LLSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_LLSHIFT_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "slr") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_LRSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_LRSHIFT_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "sub") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_SUB_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_SUB_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "subf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_SUBF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_SUBF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "subf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_SUBF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_SUBF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
                   instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_XOR_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT &&
                   instruct->args->next->data.type == ASM_ARG_ADDR &&
                   instruct->args->next->next->data.type == ASM_ARG_ADDR) {
            DECODE_INST3(OP_XOR_REG_REG, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor8") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor16") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG &&
            instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR64_REG_LIT, T_u8, T_u64);
        } else
            return ASM_ERR_ARGS;
    } else
        return ASM_ERR_MNEMONIC;
    return ASM_ERR_NONE;
}

int write_instruction(void *buf, struct AsmInstruction *instruct) {
    switch (instruct->opcode) {
        case OP_HALT:
            BUF_WRITEK(instruct->offset, OPCODE_T, OP_HALT);
            break;
        case OP_NOP:
            BUF_WRITEK(instruct->offset, OPCODE_T, OP_NOP);
            break;
        case OP_ADD_REG_LIT:
            WRITE_INST2(OP_ADD_REG_LIT, T_u8, WORD_T);
            break;
        case OP_ADD_REG_REG:
            WRITE_INST2(OP_ADD_REG_REG, T_u8, T_u8);
            break;
        case OP_ADD_MEM_MEM:
            WRITE_INST3(OP_ADD_MEM_MEM, T_u8, UWORD_T, UWORD_T);
            break;
        case OP_ADDF32_REG_LIT:
            WRITE_INST2(OP_ADDF32_REG_LIT, T_u8, T_f32);
            break;
        case OP_ADDF32_REG_REG:
            WRITE_INST2(OP_ADDF32_REG_REG, T_u8, T_u8);
            break;
        case OP_ADDF64_REG_LIT:
            WRITE_INST2(OP_ADDF64_REG_LIT, T_u8, T_f64);
            break;
        case OP_ADDF64_REG_REG:
            WRITE_INST2(OP_ADDF64_REG_REG, T_u8, T_u8);
            break;
        case OP_AND_REG_LIT:
            WRITE_INST2(OP_AND_REG_LIT, T_u8, WORD_T);
            break;
        case OP_AND_REG_REG:
            WRITE_INST2(OP_AND_REG_REG, T_u8, T_u8);
            break;
        case OP_AND_MEM_MEM:
            WRITE_INST3(OP_AND_MEM_MEM, T_u8, UWORD_T, UWORD_T);
            break;
        case OP_AND8_REG_LIT:
            WRITE_INST2(OP_AND8_REG_LIT, T_u8, T_u8);
            break;
        case OP_AND16_REG_LIT:
            WRITE_INST2(OP_AND16_REG_LIT, T_u8, T_u16);
            break;
        case OP_AND32_REG_LIT:
            WRITE_INST2(OP_AND32_REG_LIT, T_u8, T_u32);
            break;
        case OP_AND64_REG_LIT:
            WRITE_INST2(OP_AND64_REG_LIT, T_u8, T_u64);
            break;
        case OP_CALL_LIT:
            WRITE_INST1(OP_CALL_LIT, UWORD_T);
            break;
        case OP_CALL_REG:
            WRITE_INST1(OP_CALL_REG, T_u8);
            break;
        case OP_CVT_i8_i16:
            WRITE_INST1(OP_CVT_i8_i16, T_u8);
            break;
        case OP_CVT_i16_i8:
            WRITE_INST1(OP_CVT_i16_i8, T_u8);
            break;
        case OP_CVT_i16_i32:
            WRITE_INST1(OP_CVT_i16_i32, T_u8);
            break;
        case OP_CVT_i32_i16:
            WRITE_INST1(OP_CVT_i32_i16, T_u8);
            break;
        case OP_CVT_i32_i64:
            WRITE_INST1(OP_CVT_i32_i64, T_u8);
            break;
        case OP_CVT_i64_i32:
            WRITE_INST1(OP_CVT_i64_i32, T_u8);
            break;
        case OP_CVT_i32_f32:
            WRITE_INST1(OP_CVT_i32_f32, T_u8);
            break;
        case OP_CVT_f32_i32:
            WRITE_INST1(OP_CVT_f32_i32, T_u8);
            break;
        case OP_CVT_i64_f64:
            WRITE_INST1(OP_CVT_i64_f64, T_u8);
            break;
        case OP_CVT_f64_i64:
            WRITE_INST1(OP_CVT_f64_i64, T_u8);
            break;
        case OP_CMP_REG_LIT:
            WRITE_INST2(OP_CMP_REG_LIT, T_u8, WORD_T);
            break;
        case OP_CMP_REG_REG:
            WRITE_INST2(OP_CMP_REG_REG, T_u8, T_u8);
            break;
        case OP_CMP_MEM_MEM:
            WRITE_INST3(OP_CMP_MEM_MEM, T_u8, UWORD_T, UWORD_T);
            break;
        case OP_CMPF32_REG_LIT:
            WRITE_INST2(OP_CMPF32_REG_LIT, T_u8, T_f32);
            break;
        case OP_CMPF32_REG_REG:
            WRITE_INST2(OP_CMPF32_REG_REG, T_u8, T_u8);
            break;
        case OP_CMPF64_REG_LIT:
            WRITE_INST2(OP_CMPF64_REG_LIT, T_u8, T_f64);
            break;
        case OP_CMPF64_REG_REG:
            WRITE_INST2(OP_CMPF64_REG_REG, T_u8, T_u8);
            break;
        case OP_DIV_REG_LIT:
            WRITE_INST2(OP_DIV_REG_LIT, T_u8, WORD_T);
            break;
        case OP_DIV_REG_REG:
            WRITE_INST2(OP_DIV_REG_REG, T_u8, T_u8);
            break;
        case OP_DIVF32_REG_LIT:
            WRITE_INST2(OP_DIVF32_REG_LIT, T_u8, T_f32);
            break;
        case OP_DIVF32_REG_REG:
            WRITE_INST2(OP_DIVF32_REG_REG, T_u8, T_u8);
            break;
        case OP_DIVF64_REG_LIT:
            WRITE_INST2(OP_DIVF64_REG_LIT, T_u8, T_f64);
            break;
        case OP_DIVF64_REG_REG:
            WRITE_INST2(OP_DIVF64_REG_REG, T_u8, T_u8);
            break;
        case OP_GET_CHAR:
            WRITE_INST1(OP_GET_CHAR, T_u8);
            break;
        case OP_JMP_REG:
            WRITE_INST1(OP_JMP_REG, T_u8);
            break;
        case OP_JMP_ADDR:
            WRITE_INST1(OP_JMP_ADDR, UWORD_T);
            break;
        case OP_JMP_EQ_REG:
            WRITE_INST1(OP_JMP_EQ_REG, T_u8);
            break;
        case OP_JMP_EQ_ADDR:
            WRITE_INST1(OP_JMP_EQ_ADDR, UWORD_T);
            break;
        case OP_JMP_NEQ_REG:
            WRITE_INST1(OP_JMP_NEQ_REG, T_u8);
            break;
        case OP_JMP_NEQ_ADDR:
            WRITE_INST1(OP_JMP_NEQ_ADDR, UWORD_T);
            break;
        case OP_JMP_LT_REG:
            WRITE_INST1(OP_JMP_LT_REG, T_u8);
            break;
        case OP_JMP_LT_ADDR:
            WRITE_INST1(OP_JMP_LT_ADDR, UWORD_T);
            break;
        case OP_JMP_GT_REG:
            WRITE_INST1(OP_JMP_GT_REG, T_u8);
            break;
        case OP_JMP_GT_ADDR:
            WRITE_INST1(OP_JMP_GT_ADDR, UWORD_T);
            break;
        case OP_MOV_LIT_REG:
            WRITE_INST2(OP_MOV_LIT_REG, WORD_T, T_u8);
            break;
        case OP_MOV_LIT_MEM:
            WRITE_INST2(OP_MOV_LIT_MEM, WORD_T, UWORD_T);
            break;
        case OP_MOV_MEM_REG:
            WRITE_INST2(OP_MOV_MEM_REG, UWORD_T, T_u8);
            break;
        case OP_MOV_REG_MEM:
            WRITE_INST2(OP_MOV_REG_MEM, T_u8, UWORD_T);
            break;
        case OP_MOV_REGPTR_REG:
            WRITE_INST2(OP_MOV_REGPTR_REG, T_u8, T_u8);
            break;
        case OP_MOV_REG_REGPTR:
            WRITE_INST2(OP_MOV_REG_REGPTR, T_u8, T_u8);
            break;
        case OP_MOV_REG_REG:
            WRITE_INST2(OP_MOV_REG_REG, T_u8, T_u8);
            break;
        case OP_MOV8_MEM_REG:
            WRITE_INST2(OP_MOV8_MEM_REG, T_u8, WORD_T);
            break;
        case OP_MOV8_REG_MEM:
            WRITE_INST2(OP_MOV8_REG_MEM, T_u8, WORD_T);
            break;
        case OP_MOV8_REGPTR_REG:
            WRITE_INST2(OP_MOV8_REGPTR_REG, T_u8, T_u8);
            break;
        case OP_MOV8_REG_REGPTR:
            WRITE_INST2(OP_MOV8_REG_REGPTR, T_u8, T_u8);
            break;
        case OP_MOV16_MEM_REG:
            WRITE_INST2(OP_MOV16_MEM_REG, T_u8, WORD_T);
            break;
        case OP_MOV16_REG_MEM:
            WRITE_INST2(OP_MOV16_REG_MEM, T_u8, WORD_T);
            break;
        case OP_MOV16_REGPTR_REG:
            WRITE_INST2(OP_MOV16_REGPTR_REG, T_u8, T_u8);
            break;
        case OP_MOV16_REG_REGPTR:
            WRITE_INST2(OP_MOV16_REG_REGPTR, T_u8, T_u8);
            break;
        case OP_MOV32_MEM_REG:
            WRITE_INST2(OP_MOV32_MEM_REG, T_u8, WORD_T);
            break;
        case OP_MOV32_REG_MEM:
            WRITE_INST2(OP_MOV32_REG_MEM, T_u8, WORD_T);
            break;
        case OP_MOV32_REGPTR_REG:
            WRITE_INST2(OP_MOV32_REGPTR_REG, T_u8, T_u8);
            break;
        case OP_MOV32_REG_REGPTR:
            WRITE_INST2(OP_MOV32_REG_REGPTR, T_u8, T_u8);
            break;
        case OP_MOV64_MEM_REG:
            WRITE_INST2(OP_MOV64_MEM_REG, T_u8, WORD_T);
            break;
        case OP_MOV64_REG_MEM:
            WRITE_INST2(OP_MOV64_REG_MEM, T_u8, WORD_T);
            break;
        case OP_MOV64_REGPTR_REG:
            WRITE_INST2(OP_MOV64_REGPTR_REG, T_u8, T_u8);
            break;
        case OP_MOV64_REG_REGPTR:
            WRITE_INST2(OP_MOV64_REG_REGPTR, T_u8, T_u8);
            break;
        case OP_MUL_REG_LIT:
            WRITE_INST2(OP_MUL_REG_LIT, T_u8, WORD_T);
            break;
        case OP_MUL_REG_REG:
            WRITE_INST2(OP_MUL_REG_REG, T_u8, T_u8);
            break;
        case OP_MULF32_REG_LIT:
            WRITE_INST2(OP_MULF32_REG_LIT, T_u8, T_f32);
            break;
        case OP_MULF32_REG_REG:
            WRITE_INST2(OP_MULF32_REG_REG, T_u8, T_u8);
            break;
        case OP_MULF64_REG_LIT:
            WRITE_INST2(OP_MULF64_REG_LIT, T_u8, T_f64);
            break;
        case OP_MULF64_REG_REG:
            WRITE_INST2(OP_MULF64_REG_REG, T_u8, T_u8);
            break;
        case OP_NEG:
            WRITE_INST1(OP_NEG, T_u8);
            break;
        case OP_NEGF32:
            WRITE_INST1(OP_NEGF32, T_u8);
            break;
        case OP_NEGF64:
            WRITE_INST1(OP_NEGF64, T_u8);
            break;
        case OP_NOT_REG:
            WRITE_INST1(OP_NOT_REG, T_u8);
            break;
        case OP_NOT_MEM:
            WRITE_INST2(OP_NOT_MEM, T_u8, UWORD_T);
            break;
        case OP_OR_REG_LIT:
            WRITE_INST2(OP_OR_REG_LIT, T_u8, WORD_T);
            break;
        case OP_OR_REG_REG:
            WRITE_INST2(OP_OR_REG_REG, T_u8, T_u8);
            break;
        case OP_OR_MEM_MEM:
            WRITE_INST3(OP_OR_MEM_MEM, T_u8, UWORD_T, UWORD_T);
            break;
        case OP_OR8_REG_LIT:
            WRITE_INST2(OP_OR8_REG_LIT, T_u8, T_u8);
            break;
        case OP_OR16_REG_LIT:
            WRITE_INST2(OP_OR16_REG_LIT, T_u8, T_u16);
            break;
        case OP_OR32_REG_LIT:
            WRITE_INST2(OP_OR32_REG_LIT, T_u8, T_u32);
            break;
        case OP_OR64_REG_LIT:
            WRITE_INST2(OP_OR64_REG_LIT, T_u8, T_u64);
            break;
        case OP_POP_REG:
            WRITE_INST1(OP_POP_REG, T_u8);
            break;
        case OP_POP_REGPTR:
            WRITE_INST1(OP_POP_REGPTR, T_u8);
            break;
        case OP_POPN_REGPTR:
            WRITE_INST2(OP_POPN_REGPTR, T_u8, T_u8);
            break;
        case OP_POPN_MEM:
            WRITE_INST2(OP_POPN_MEM, T_u8, UWORD_T);
            break;
        case OP_POP8_REG:
            WRITE_INST1(OP_POP8_REG, T_u8);
            break;
        case OP_POP8_REGPTR:
            WRITE_INST1(OP_POP8_REGPTR, T_u8);
            break;
        case OP_POP16_REG:
            WRITE_INST1(OP_POP16_REG, T_u8);
            break;
        case OP_POP16_REGPTR:
            WRITE_INST1(OP_POP16_REGPTR, T_u8);
            break;
        case OP_POP32_REG:
            WRITE_INST1(OP_POP32_REG, T_u8);
            break;
        case OP_POP32_REGPTR:
            WRITE_INST1(OP_POP32_REGPTR, T_u8);
            break;
        case OP_POP64_REG:
            WRITE_INST1(OP_POP64_REG, T_u8);
            break;
        case OP_POP64_REGPTR:
            WRITE_INST1(OP_POP64_REGPTR, T_u8);
            break;
        case OP_PRINT_CHARS_MEM:
            WRITE_INST2(OP_PRINT_CHARS_MEM, T_u8, UWORD_T);
            break;
        case OP_PRINT_CHARS_REG:
            WRITE_INST1(OP_PRINT_CHARS_REG, T_u8);
            break;
        case OP_PRINT_HEX_MEM:
            WRITE_INST2(OP_PRINT_HEX_MEM, T_u8, UWORD_T);
            break;
        case OP_PRINT_HEX_REG:
            WRITE_INST1(OP_PRINT_HEX_REG, T_u8);
            break;
        case OP_PRINT_DBL_REG:
            WRITE_INST1(OP_PRINT_DBL_REG, T_u8);
            break;
        case OP_PRINT_INT_REG:
            WRITE_INST1(OP_PRINT_INT_REG, T_u8);
            break;
        case OP_PRINT_UINT_REG:
            WRITE_INST1(OP_PRINT_UINT_REG, T_u8);
            break;
        case OP_PSTACK:
            BUF_WRITEK(instruct->offset, OPCODE_T, OP_PSTACK);
            break;
        case OP_PREG:
            BUF_WRITEK(instruct->offset, OPCODE_T, OP_PREG);
            break;
        case OP_PUSH_LIT:
            WRITE_INST1(OP_PUSH_LIT, WORD_T);
            break;
        case OP_PUSH_MEM:
            WRITE_INST1(OP_PUSH_MEM, UWORD_T);
            break;
        case OP_PUSHN_MEM:
            WRITE_INST2(OP_PUSHN_MEM, T_u8, UWORD_T);
            break;
        case OP_PUSH_REG:
            WRITE_INST1(OP_PUSH_REG, T_u8);
            break;
        case OP_PUSH_REGPTR:
            WRITE_INST1(OP_PUSH_REGPTR, T_u8);
            break;
        case OP_PUSHN_REGPTR:
            WRITE_INST2(OP_PUSHN_REGPTR, T_u8, T_u8);
            break;
        case OP_PUSH8_LIT:
            WRITE_INST1(OP_PUSH8_LIT, T_u8);
            break;
        case OP_PUSH8_MEM:
            WRITE_INST1(OP_PUSH8_MEM, UWORD_T);
            break;
        case OP_PUSH8_REG:
            WRITE_INST1(OP_PUSH8_REG, T_u8);
            break;
        case OP_PUSH8_REGPTR:
            WRITE_INST1(OP_PUSH8_REGPTR, T_u8);
            break;
        case OP_PUSH16_LIT:
            WRITE_INST1(OP_PUSH16_LIT, T_u16);
            break;
        case OP_PUSH16_MEM:
            WRITE_INST1(OP_PUSH16_MEM, UWORD_T);
            break;
        case OP_PUSH16_REG:
            WRITE_INST1(OP_PUSH16_REG, T_u8);
            break;
        case OP_PUSH16_REGPTR:
            WRITE_INST1(OP_PUSH16_REGPTR, T_u8);
            break;
        case OP_PUSH32_LIT:
            WRITE_INST1(OP_PUSH32_LIT, T_u32);
            break;
        case OP_PUSH32_MEM:
            WRITE_INST1(OP_PUSH32_MEM, UWORD_T);
            break;
        case OP_PUSH32_REG:
            WRITE_INST1(OP_PUSH32_REG, T_u8);
            break;
        case OP_PUSH32_REGPTR:
            WRITE_INST1(OP_PUSH32_REGPTR, T_u8);
            break;
        case OP_PUSH64_LIT:
            WRITE_INST1(OP_PUSH64_LIT, T_u64);
            break;
        case OP_PUSH64_MEM:
            WRITE_INST1(OP_PUSH64_MEM, UWORD_T);
            break;
        case OP_PUSH64_REG:
            WRITE_INST1(OP_PUSH64_REG, T_u8);
            break;
        case OP_PUSH64_REGPTR:
            WRITE_INST1(OP_PUSH64_REGPTR, T_u8);
            break;
        case OP_RET:
            BUF_WRITEK(instruct->offset, OPCODE_T, OP_RET);
            break;
        case OP_ARSHIFT_LIT:
            WRITE_INST2(OP_ARSHIFT_LIT, T_u8, T_u8);
            break;
        case OP_ARSHIFT_REG:
            WRITE_INST2(OP_ARSHIFT_REG, T_u8, T_u8);
            break;
        case OP_LLSHIFT_LIT:
            WRITE_INST2(OP_LLSHIFT_LIT, T_u8, T_u8);
            break;
        case OP_LLSHIFT_REG:
            WRITE_INST2(OP_LLSHIFT_REG, T_u8, T_u8);
            break;
        case OP_LRSHIFT_LIT:
            WRITE_INST2(OP_LRSHIFT_LIT, T_u8, T_u8);
            break;
        case OP_LRSHIFT_REG:
            WRITE_INST2(OP_LRSHIFT_REG, T_u8, T_u8);
            break;
        case OP_SUB_REG_LIT:
            WRITE_INST2(OP_SUB_REG_LIT, T_u8, WORD_T);
            break;
        case OP_SUB_REG_REG:
            WRITE_INST2(OP_SUB_REG_REG, T_u8, T_u8);
            break;
        case OP_SUBF32_REG_LIT:
            WRITE_INST2(OP_SUBF32_REG_LIT, T_u8, T_f32);
            break;
        case OP_SUBF32_REG_REG:
            WRITE_INST2(OP_SUBF32_REG_REG, T_u8, T_u8);
            break;
        case OP_SUBF64_REG_LIT:
            WRITE_INST2(OP_SUBF64_REG_LIT, T_u8, T_f64);
            break;
        case OP_SUBF64_REG_REG:
            WRITE_INST2(OP_SUBF64_REG_REG, T_u8, T_u8);
            break;
        case OP_XOR_REG_LIT:
            WRITE_INST2(OP_XOR_REG_LIT, T_u8, WORD_T);
            break;
        case OP_XOR_REG_REG:
            WRITE_INST2(OP_XOR_REG_REG, T_u8, T_u8);
            break;
        case OP_XOR_MEM_MEM:
            WRITE_INST3(OP_XOR_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        case OP_XOR8_REG_LIT:
            WRITE_INST2(OP_XOR8_REG_LIT, T_u8, T_u8);
            break;
        case OP_XOR16_REG_LIT:
            WRITE_INST2(OP_XOR16_REG_LIT, T_u8, T_u16);
            break;
        case OP_XOR32_REG_LIT:
            WRITE_INST2(OP_XOR32_REG_LIT, T_u8, T_u32);
            break;
        case OP_XOR64_REG_LIT:
            WRITE_INST2(OP_XOR64_REG_LIT, T_u8, T_u64);
            break;
        default:
            return ASM_ERR_UNK_OPCODE;
    }
    return ASM_ERR_NONE;
}