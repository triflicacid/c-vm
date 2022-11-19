#include "assemble.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cpu/registers.h"
#include "../cpu/opcodes.h"
#include "args.h"
#include "chunk.h"
#include "err.h"
#include "instruction.h"
#include "labels.h"
#include "line.h"
#include "parse_data.h"
#include "symbol.h"

struct AsmError asm_error_create()
{
    struct AsmError err = { .line = 0, .col = 0, .errc = ASM_ERR_NONE, .print = 0, .debug = 0 };
    return err;
}

struct AsmData asm_data_create()
{
    struct AsmData data = {
        .stage = ASM_STAGE_NONE, .bytes = 0, .lines = 0, .symbols = 0, .labels = 0, .chunks = 0
    };
    return data;
}

void asm_data_destroy(struct AsmData* data)
{
    if (data->lines != 0)
        linked_list_destroy_AsmLine(&(data->lines));
    if (data->symbols != 0)
        linked_list_destroy_AsmLine(&(data->lines));
    if (data->labels != 0)
        linked_list_destroy_AsmLabel(&(data->labels));
    if (data->chunks != 0)
        linked_list_destroy_AsmChunk(&(data->chunks));
    data->stage = ASM_STAGE_NONE;
}

void asm_read_lines(FILE* fp, struct AsmData* data, struct AsmError* err)
{
    data->stage = ASM_STAGE_LINES;
    data->lines = 0;
    char string[ASM_MAX_LINE_LENGTH]; // Line buffer
    unsigned int line = 0; // Line number

    while (1) {
        if (!fgets(string, sizeof(string), fp))
            break;
        unsigned int len = 0, i;
        // Calculate string length
        for (i = 0;
             !(string[i] == '\0' || string[i] == '\n' || string[i] == '\r' || string[i] == ';');
             ++i, ++len)
            ;
        i = 0;
        // Eat leading whitespace
        for (; i < len && IS_WHITESPACE(string[i]); ++i)
            ;
        // Create node?
        if (i < len) {
            char* str = extract_string(string, 0, len);

            struct LL_NODET_NAME(AsmLine)* node = malloc(sizeof(struct LL_NODET_NAME(AsmLine)));
            node->data.n = line;
            node->data.len = len;
            node->data.str = str;
            linked_list_insertnode_AsmLine(node, &(data->lines), -1);
        }
        ++line;
    }
}

unsigned long long asm_write_lines(struct LL_NODET_NAME(AsmLine) * lines, char** buffer)
{
    unsigned long long bytes = 0;
    struct LL_NODET_NAME(AsmLine)* line = lines;
    while (line != 0) {
        bytes += line->data.len + 1;
        line = line->next;
    }

    line = lines;
    *buffer = malloc(bytes + 1);
    unsigned int offset = 0;
    while (line != 0) {
        memcpy(*buffer + offset, line->data.str, line->data.len);
        offset += line->data.len;
        (*buffer)[offset++] = '\n';
        line = line->next;
    }
    (*buffer)[bytes] = '\0';
    return bytes + 1;
}

void asm_preprocess(struct AsmData* data, struct AsmError* err)
{
    data->stage = ASM_STAGE_PREPROC;
    struct LL_NODET_NAME(AsmLine)* cline = data->lines;
    while (cline != 0) {
        unsigned int idx = 0;
        unsigned int slen = cline->data.len;
        char* string = cline->data.str;

        // Eat leading ws
        while (idx < slen && IS_WHITESPACE(string[idx]))
            ++idx;

        // Replace constants?
        unsigned int idx2 = idx;
        struct LL_NODET_NAME(AsmSymbol)* csym = data->symbols;
        while (csym != 0) {
            unsigned int len = strlen(csym->data.name), vlen = strlen(csym->data.value);
            while (idx2 < slen) {
                // Eas ws
                while (idx2 < slen && IS_WHITESPACE(string[idx2]))
                    ++idx2;
                if (idx2 == slen)
                    break;

                unsigned int match = 1, idxn = 0;
                while (idxn < len && match && !IS_WHITESPACE(string[idx2 + idxn])) {
                    if (csym->data.name[idxn] != string[idx2 + idxn])
                        match = 0;
                    else
                        ++idxn;
                }
                if (match) {
                    if (err->debug)
                        printf("FOUND match for symbol \"%s\" at %u:%u\n", csym->data.name,
                            cline->data.n, idx2);
                    unsigned int new_len = slen - len + vlen;
                    char* new_str = malloc(new_len);
                    memcpy(new_str, string, idx2);
                    memcpy(new_str + idx2, csym->data.value, vlen);
                    memcpy(new_str + idx2 + vlen, string + idx2 + len, slen - idx2);
                    free(string);
                    cline->data.str = string = new_str;
                    cline->data.len = slen = new_len;
                }
                // Skip to next ws
                while (idx2 < slen && !IS_WHITESPACE(string[idx2]))
                    ++idx2;
            }
            csym = csym->next;
        }

        if (string[idx] == '%') { // Directive
            unsigned int didx = idx, dlen = 0; // Directive index/length
            while (idx < slen && !IS_WHITESPACE(string[idx]))
                ++idx, ++dlen;

            char* directive = extract_string(string, didx + 1, dlen - 1);
            if (strcmp(directive, "define") == 0) { // %define
                while (idx < slen && IS_WHITESPACE(string[idx]))
                    ++idx;
                // Extract symbol name
                unsigned int name_i = idx, name_len = 0;
                while (idx < slen && !IS_WHITESPACE(string[idx]))
                    ++idx, ++name_len;
                char* name = extract_string(string, name_i, name_len);
                ++idx;
                unsigned int vlen = slen - idx; // Value length
                // Extract value (read to EOL)
                char* value = extract_string(string, idx, vlen);
                if (err->debug)
                    printf("[LINE %u] DEFINE \"%s\" TO BE \"%s\"\n", cline->data.n, name, value);
                idx = slen;

                struct LL_NODET_NAME(AsmSymbol)* node
                    = malloc(sizeof(struct LL_NODET_NAME(AsmSymbol)));
                node->data.name = name;
                node->data.value = value;
                linked_list_insertnode_AsmSymbol(node, &(data->symbols), -1);
            } else if (strcmp(directive, "ignore") == 0) { // %ignore
                if (err->debug)
                    printf("[LINE %u] %%%s: IGNORE THIS LINE\n", cline->data.n, directive);
                // Ignore this line - it'll get removed at the end, anyway
            } else if (strcmp(directive, "stop") == 0) { // %stop
                if (err->debug)
                    printf("[LINE %u] %%stop: IGNORE PAST THIS POINT\n", cline->data.n);
                // Remove this line and all lines past this point
                struct LL_NODET_NAME(AsmLine)* line = data->lines, *next = 0;
                while (line != 0 && line->next != cline)
                    line = line->next;
                line->next = 0;
                line = cline;
                while (line != 0) {
                    next = line->next;
                    free(line->data.str);
                    free(line);
                    line = next;
                }
                break;
            } else {
                if (err->print)
                    printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                       " Line %i, column %i:\nUnknown directive "
                                       "%%%s\n",
                        cline->data.n, idx, directive);
                free(directive);
                err->errc = ASM_ERR_DIRECTIVE;
                return;
            }
            free(directive);
            // Remote current line - it is done
            struct LL_NODET_NAME(AsmLine)* next = cline->next;
            linked_list_removenode_AsmLine(cline, &(data->lines));
            free(cline->data.str);
            free(cline);
            cline = next;
            continue;
        }
        cline = cline->next;
    }
}

void asm_parse(struct AsmData* data, struct AsmError* err)
{
    data->stage = ASM_STAGE_PARSE;
    unsigned int offset = 0;
    struct LL_NODET_NAME(AsmLine)* cline = data->lines;
    while (cline != 0) {
        int pos = 0;
        const unsigned int slen = cline->data.len, line = cline->data.n;
        const char* string = cline->data.str;

        // Eat leading whitespace
        for (; pos < slen && IS_WHITESPACE(string[pos]); ++pos)
            ;

        // Empty line?
        if (pos == slen) {
            if (err->debug)
                printf("Line %i, length %i (empty)\n", line, slen);
            continue;
        }

        // Get first item
        int mptr = pos, mlen = 0;
        for (; pos < slen && !IS_WHITESPACE(string[pos]); ++pos, ++mlen)
            ;

        // LABEL
        if (string[mptr + mlen - 1] == ':') {
            char* lbl = extract_string(string, mptr, mlen - 1); // Label string
            if (err->debug) {
                printf("Label \"%s\" at offset +%u\n", lbl, offset);
            }
            // Does label already exist?
            struct AsmLabel* label = linked_list_find_AsmLabel(data->labels, lbl);
            if (label == 0) { // Create new label
                struct LL_NODET_NAME(AsmLabel)* node
                    = malloc(sizeof(struct LL_NODET_NAME(AsmLabel)));
                node->data.ptr = lbl;
                node->data.len = mlen - 1;
                node->data.addr = offset;
                linked_list_insertnode_AsmLabel(node, &(data->labels), -1);

                // Replace past references to this label
                struct LL_NODET_NAME(AsmChunk)* chunk = data->chunks;
                while (chunk != 0) {
                    // If chunk is storing an instruction
                    if (chunk->data.type == ASM_CHUNKT_INSTRUCTION) {
                        struct AsmInstruction* instruct = chunk->data.data;
                        struct LL_NODET_NAME(AsmArgument)* arg = instruct->args;
                        unsigned int i = 0;
                        while (arg != 0) {
                            if (arg->data.type == ASM_ARG_LABEL
                                && strcmp(lbl, (char*)arg->data.data) == 0) {
                                if (err->debug)
                                    printf("Mnemonic \"%s\"/Opcode %u, arg %u: "
                                           "label \"%s\" "
                                           "-> addr [%llu]\n",
                                        instruct->mnemonic, instruct->opcode, i, lbl, offset);
                                arg->data.type = ASM_ARG_ADDR;
                                arg->data.data = offset;
                            }
                            ++i;
                            arg = arg->next;
                        }
                    }
                    chunk = chunk->next;
                }
            } else { // Update label
                label->addr = offset;
                free(lbl);
            }
            int size = 0;
            for (; pos < slen && IS_WHITESPACE(string[pos]); ++pos)
                ;
            if (pos == slen) { // End of line?
                cline = cline->next;
                continue;
            }
            for (mptr = pos, mlen = 0; pos < slen && !IS_WHITESPACE(string[pos]); ++pos, ++mlen)
                ;
        }

        // Create data
        struct LL_NODET_NAME(AsmChunk)* chunk_node = malloc(sizeof(struct LL_NODET_NAME(AsmChunk)));
        chunk_node->next = 0;
        struct AsmChunk* chunk = &(chunk_node->data);
        chunk->offset = offset;
        chunk->bytes = 0;

        // Get mnemonic
        char* mnemonic = malloc(mlen + 1);
        memcpy(mnemonic, string + mptr, mlen);
        mnemonic[mlen] = '\0';

        if (err->debug) {
            printf("Line %i, length %i, mnemonic '%s'\n", line, slen, mnemonic);
        }

        // Eat whitespace
        for (; pos < slen && IS_WHITESPACE(string[pos]); ++pos)
            ;

        if (strcmp(mnemonic, "u8") == 0) {
            PARSEDATA_EXTRACT(u8, T_u8);
        } else if (strcmp(mnemonic, "u16") == 0) {
            PARSEDATA_EXTRACT(u16, T_u16);
        } else if (strcmp(mnemonic, "u32") == 0) {
            PARSEDATA_EXTRACT(u32, T_u32);
        } else if (strcmp(mnemonic, "u64") == 0) {
            PARSEDATA_EXTRACT(u64, T_u64);
        } else if (strcmp(mnemonic, "f32") == 0) {
            PARSEDATA_EXTRACT(f32, T_f32);
        } else if (strcmp(mnemonic, "f64") == 0) {
            PARSEDATA_EXTRACT(f64, T_f64);
        } else { // Initialise instruction
            chunk->type = ASM_CHUNKT_INSTRUCTION;
            chunk->data = malloc(sizeof(struct AsmInstruction));
            struct AsmInstruction* instruct = chunk->data;
            instruct->args = 0;
            instruct->bytes = 0;
            instruct->opcode = 0;
            instruct->mnemonic = mnemonic;

            // Get arguments
            while (pos < slen) {
                // Eat whitespace
                for (; pos < slen && IS_WHITESPACE(string[pos]); ++pos)
                    ;
                if (pos == slen)
                    break;

                if (string[pos] == '\'') { // CHARACTER LITERAL
                    int j = 0, k = 0;
                    char data[sizeof(UWORD_T)] = { 0 };
                    while (j < sizeof(data) && pos < slen && string[pos] == '\'') {
                        if (string[pos + 1] == '\\') { // Escape sequence
                            char* ptr = (char*)string + pos + 2;
                            long long val = decode_escape_seq(&ptr);
                            data[j] = (char)val;
                            pos = ptr - string + 1;
                        } else {
                            if (string[pos + 2] == '\'') {
                                data[j] = string[pos + 1];
                                pos += 3;
                            } else {
                                if (err->print) {
                                    char astr[] = { string[pos], string[pos + 1], '\0' };
                                    printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                                       " Line %i, column %i:\nExpected ' "
                                                       "after character expression "
                                                       "%s <-- '\n",
                                        line, pos, astr);
                                }
                                err->col = pos;
                                err->line = cline->data.n;
                                err->errc = ASM_ERR_GENERIC;
                                asm_free_instruction_chunk(chunk);
                                free(chunk_node);
                                return;
                            }
                        }
                        j++;
                        CONSUME_WHITESPACE(string, pos);
                    }

                    struct LL_NODET_NAME(AsmArgument)* node
                        = malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                    node->data.type = ASM_ARG_LIT;
                    memcpy(&(node->data.data), data, sizeof(data));
                    linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
                } else if (string[pos] == '\"') { // STRING LITERAL
                    unsigned short j = 0;
                    char data[sizeof(UWORD_T)] = { 0 };
                    ++pos;
                    while (j < sizeof(data) && pos < slen) {
                        if (string[pos] == '\"') {
                            ++pos;
                            break;
                        }
                        if (string[pos] == '\\') { // Escape sequence
                            char* ptr = (char*)string + pos + 1;
                            long long val = decode_escape_seq(&ptr);
                            data[j] = (char)val;
                            pos = ptr - string;
                        } else {
                            data[j] = string[pos];
                            ++pos;
                        }
                        j++;
                    }

                    struct LL_NODET_NAME(AsmArgument)* node
                        = malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                    node->data.type = ASM_ARG_LIT;
                    memcpy(&(node->data.data), data, sizeof(data));
                    linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
                } else if (string[pos] == '[') { // Address/Register pointer
                    unsigned int len = 0;
                    while (pos + len < slen && !(IS_WHITESPACE(string[pos + len]) || IS_SEPERATOR(string[pos + len])))
                        ++len;
                    if (string[pos + len - 1] != ']') {
                        if (err->print) {
                            char* astr = extract_string(string, pos, len);
                            printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                               " Line %i, column %i:\nExpected ']' after "
                                               "address expression: '%s' <-- ]\n",
                                line, pos, astr);
                            free(astr);
                        }
                        err->col = pos;
                        err->line = cline->data.n;
                        err->errc = ASM_ERR_ADDR;
                        asm_free_instruction_chunk(chunk);
                        free(chunk_node);
                        return;
                    }
                    if (IS_CHAR(string[pos + 1])) { // Register pointer?
                        T_i8 reg_off = cpu_reg_offset_from_string((char*)string + pos + 1);
                        if (reg_off == -1) { // Unknown register - label?
                            // Exists?
                            char* sub = extract_string(string, pos + 1, len - 2);
                            struct AsmLabel* lbl = linked_list_find_AsmLabel(data->labels, sub);
                            struct LL_NODET_NAME(AsmArgument)* node
                                = malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                            if (lbl == 0) {
                                node->data.type = ASM_ARG_LABEL;
                                node->data.data = (unsigned long long)sub;
                            } else {
                                node->data.type = ASM_ARG_ADDR;
                                node->data.data = lbl->addr;
                                free(sub);
                            }
                            linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
                        } else { // Register pointer
                            struct LL_NODET_NAME(AsmArgument)* node
                                = malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                            node->data.type = ASM_ARG_REGPTR;
                            node->data.data = reg_off;
                            linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
                        }

                    } else {
                        char* astr = extract_string(string, pos + 1, len - 2);
                        int radix = get_radix(astr[len - 3]);
                        unsigned long long addr = base_to_10(astr, radix == -1 ? 10 : radix);
                        free(astr);

                        struct LL_NODET_NAME(AsmArgument)* node
                            = malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                        node->data.type = ASM_ARG_ADDR;
                        node->data.data = addr;
                        linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
                    }
                    pos += len;
                } else if (string[pos] == '-' || string[pos] == '+' || IS_DIGIT(string[pos])
                    || IS_CHAR(string[pos])) { // Literal/Register
                    unsigned int sublen = 0;
                    while (pos + sublen < slen && !IS_SEPERATOR(string[pos + sublen])
                        && !IS_WHITESPACE(string[pos + sublen]))
                        ++sublen;
                    char* sub = extract_string(string, pos, sublen);
                    // Check if register
                    T_i8 reg_off = cpu_reg_offset_from_string(sub);
                    if (reg_off == -1) { // Is it a register?
                        int radix = get_radix(sub[sublen - 1]);
                        unsigned int litend = scan_number(sub, radix == -1 ? 10 : radix);
                        int has_dp = 0;
                        for (int j = 0; j < sublen && !IS_WHITESPACE(sub[j]); ++j) {
                            if (sub[j] == '.') {
                                has_dp = 1;
                                break;
                            }
                        }
                        if (litend == sublen
                            || (radix != -1 && litend == sublen - 1)) { // Is literal!
                            struct LL_NODET_NAME(AsmArgument)* node
                                = malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                            node->data.type = ASM_ARG_LIT;
                            if (has_dp) { // Float
                                double lit = fbase_to_10(sub, radix == -1 ? 10 : radix);
                                node->data.data = *(unsigned long long*)&lit;
                            } else { // Integer
                                unsigned long long lit = base_to_10(sub, radix == -1 ? 10 : radix);
                                node->data.data = lit;
                            }
                            linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
                        } else { // Label
                            // Exists?
                            struct AsmLabel* lbl = linked_list_find_AsmLabel(data->labels, sub);
                            struct LL_NODET_NAME(AsmArgument)* node
                                = malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                            if (lbl == 0) {
                                node->data.type = ASM_ARG_LABEL;
                                node->data.data = (unsigned long long)sub;
                            } else {
                                node->data.type = ASM_ARG_ADDR;
                                node->data.data = lbl->addr;
                            }
                            linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
                        }
                    } else {
                        struct LL_NODET_NAME(AsmArgument)* node
                            = malloc(sizeof(struct LL_NODET_NAME(AsmArgument)));
                        node->data.type = ASM_ARG_REG;
                        node->data.data = reg_off;
                        linked_list_insertnode_AsmArgument(node, &(instruct->args), -1);
                    }
                    pos += sublen;
                } else { // Unknown argument form
                    if (err->print) {
                        unsigned int len = 0;
                        while (pos + len < slen && !IS_WHITESPACE(string[pos + len]))
                            ++len;
                        char* astr = extract_string(string, pos, len);
                        printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                           " Line %i, column %i:\nUnknown argument format "
                                           "'%s'\n",
                            line, pos, astr);
                        free(astr);
                    }
                    err->col = pos;
                    err->line = cline->data.n;
                    err->errc = ASM_ERR_ARG;
                    asm_free_instruction_chunk(chunk);
                    free(chunk_node);
                    return;
                }

                // End of argument
                CONSUME_WHITESPACE(string, pos);
                if (pos == slen) {
                    break;
                } else if (string[pos] == ',') {
                    ++pos;
                } else {
                    if (err->print)
                        printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                           " Line %i, column %i:\nExpected comma, found "
                                           "'%c'\n",
                            line, pos, string[pos]);
                    err->col = pos;
                    err->line = cline->data.n;
                    err->errc = ASM_ERR_GENERIC;
                    asm_free_instruction_chunk(chunk);
                    free(chunk_node);
                    return;
                }
            }

            int errc = asm_decode_instruction(instruct);
            if (errc == ASM_ERR_MNEMONIC) {
                if (err->print)
                    printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                       " Line %i, column %i:\nUnknown mnemonic '%s'\n",
                        line, pos, instruct->mnemonic);
                err->col = pos;
                err->line = cline->data.n;
                err->errc = errc;
                asm_free_instruction_chunk(chunk);
                free(chunk_node);
                return;
            } else if (errc == ASM_ERR_ARGS) {
                if (err->print) {
                    unsigned int argc = linked_list_size_AsmArgument(instruct->args);
                    printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                       " Line %i, column %i:\nUnknown argument(s) for "
                                       "\"%s\": [%u] ",
                        line, pos, instruct->mnemonic, argc);
                    struct LL_NODET_NAME(AsmArgument)* curr = instruct->args;
                    for (unsigned int i = 0; curr != 0; ++i, curr = curr->next) {
                        print_asm_arg(&(curr->data));
                        if (i < argc - 1)
                            printf("; ");
                    }
                    printf("\n");
                }
                err->col = pos;
                err->line = cline->data.n;
                err->errc = errc;
                asm_free_instruction_chunk(chunk);
                free(chunk_node);
                return;
            }

            // Read byte-length
            chunk->bytes = instruct->bytes;
        }

        if (chunk->bytes == 0) {
            // No data... ignore
            asm_destroy_chunk(chunk);
            free(chunk_node);
        } else {
            offset += chunk->bytes;
            SET_IF_LARGER(data->bytes, offset);

            // Insert chunk into linked list
            struct AsmChunk* collision
                = asm_chunk_in_range(data->chunks, chunk->offset, chunk->offset + chunk->bytes);
            if (collision == 0) {
                linked_list_insertnode_AsmChunk(&(data->chunks), chunk_node);
            } else {
                if (err->print)
                    printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                       " Line %i, column %i:\nChunk collision - %u bytes at "
                                       "%llu\n",
                        line, pos, collision->bytes, collision->offset);
                err->col = pos;
                err->line = cline->data.n;
                err->errc = ASM_ERR_MEMORY;
                asm_destroy_chunk(chunk);
                free(chunk_node);
                return;
            }
        }
        // Next line
        cline = cline->next;
    }

    // Check for labels (should be none).
    struct LL_NODET_NAME(AsmChunk)* chunk = data->chunks;
    while (chunk != 0) {
        if (chunk->data.type == ASM_CHUNKT_INSTRUCTION) {
            struct AsmInstruction* instruct = chunk->data.data;
            struct LL_NODET_NAME(AsmArgument)* arg = instruct->args;
            unsigned int i = 0;
            while (arg != 0) {
                if (arg->data.type == ASM_ARG_LABEL) {
                    if (err->print)
                        printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                           " Instruction \"%s\" (+%u): reference to "
                                           "undefined "
                                           "label \"%s\"\n",
                            instruct->mnemonic, chunk->data.offset, arg->data.data);
                    err->line = 0; // Unknown.
                    err->col = 0; // Unknown.
                    err->errc = ASM_ERR_LABEL;
                    return;
                }
                ++i;
                arg = arg->next;
            }
        }
        chunk = chunk->next;
    }
}

char* asm_compile(struct AsmData* data, struct AsmError* err)
{
    data->stage = ASM_STAGE_COMPILE;
    if (data->bytes == 0)
        return 0; // Return NULL if nothing to write
    char* buf = malloc(data->bytes); // Create byte buffer
    struct LL_NODET_NAME(AsmChunk)* chunk = data->chunks;
    while (chunk != 0) {
        switch (chunk->data.type) {
        case ASM_CHUNKT_INSTRUCTION: {
            struct AsmInstruction* instruct = chunk->data.data;
            int errc = asm_write_instruction(buf, chunk->data.offset, instruct);
            if (errc != ASM_ERR_NONE) {
                if (err->print)
                    printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                       " Unknown opcode whilst decoding: %llu\n",
                        instruct->opcode);
                err->line = 0; // Unknown.
                err->col = 0; // Unknown.
                err->errc = errc;
                free(buf);
                return 0;
            }
            break;
        }
        case ASM_CHUNKT_DATA:
            memcpy(buf + chunk->data.offset, chunk->data.data, chunk->data.bytes);
            break;
        default:
            if (err->print)
                printf(CONSOLE_RED "ERROR!" CONSOLE_RESET
                                   " Unknown data chunk type whist decoding: %i\n",
                    chunk->data.type);
            err->line = 0; // Unknown.
            err->col = 0; // Unknown.
            err->errc = ASM_ERR_GENERIC;
            free(buf);
            return 0;
        }
        chunk = chunk->next;
    }
    return buf;
}

int asm_decode_instruction(struct AsmInstruction* instruct)
{
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
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_ADD_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_ADD_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)
            && (instruct->args->next->next->data.type == ASM_ARG_ADDR || instruct->args->next->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST3(OP_ADD_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)
            && instruct->args->next->next->data.type == ASM_ARG_LIT) {
            DECODE_INST3(OP_ADD_MEM_LIT, T_u8, UWORD_T, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "addf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_ADDF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_ADDF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "addf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_ADDF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_ADDF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_AND_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)
            && (instruct->args->next->next->data.type == ASM_ARG_ADDR || instruct->args->next->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST3(OP_AND_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and8") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and16") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_AND32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "and64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
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
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_CMP_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_CMP_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)
            && (instruct->args->next->next->data.type == ASM_ARG_ADDR || instruct->args->next->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST3(OP_CMP_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "cmpf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_CMPF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_CMPF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "cmpf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_CMPF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_CMPF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "div") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_DIV_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_DIV_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "divf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_DIVF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_DIVF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "divf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_DIVF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
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
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jeq") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_EQ_REG, T_u8);
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_EQ_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jne") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_NEQ_REG, T_u8);
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_NEQ_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jlt") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_LT_REG, T_u8);
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_LT_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "jgt") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_JMP_GT_REG, T_u8);
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_JMP_GT_ADDR, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV_LIT_REG, WORD_T, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_MOV_LIT_MEM, WORD_T, UWORD_T);
        } else if (argc == 2
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV_MEM_REG, UWORD_T, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_MOV_REG_MEM, T_u8, UWORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV_REG_REGPTR, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov8") == 0) {
        if (argc == 2
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV8_MEM_REG, UWORD_T, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_MOV8_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV8_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV8_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov16") == 0) {
        if (argc == 2
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV16_MEM_REG, UWORD_T, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_MOV16_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV16_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV16_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov32") == 0) {
        if (argc == 2
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV32_MEM_REG, UWORD_T, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_MOV32_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV32_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV32_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mov64") == 0) {
        if (argc == 2
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV64_MEM_REG, UWORD_T, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_MOV64_REG_MEM, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REGPTR
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MOV64_REGPTR_REG, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_MOV64_REG_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mul") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_MUL_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MUL_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mulf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_MULF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_MULF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "mulf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_MULF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
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
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_NOT_MEM, T_u8, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_OR_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)
            && (instruct->args->next->next->data.type == ASM_ARG_ADDR || instruct->args->next->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST3(OP_OR_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or8") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or16") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "or64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_OR64_REG_LIT, T_u8, T_u64);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "pop") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_POP_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_POP_REGPTR, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_POPN_REGPTR, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
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
    } else if (strcmp(instruct->mnemonic, "prb") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PRINT_BIN_REG, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_PRINT_BIN_MEM, T_u8, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "prc") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
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
        if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
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
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
            DECODE_INST1(OP_PUSH_MEM, UWORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST2(OP_PUSHN_MEM, T_u8, UWORD_T);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REG) {
            DECODE_INST1(OP_PUSH_REG, T_u8);
        } else if (argc == 1 && instruct->args->data.type == ASM_ARG_REGPTR) {
            DECODE_INST1(OP_PUSH_REGPTR, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_LIT
            && instruct->args->next->data.type == ASM_ARG_REGPTR) {
            DECODE_INST2(OP_PUSHN_REGPTR, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "psh8") == 0) {
        if (argc == 1 && instruct->args->data.type == ASM_ARG_LIT) {
            DECODE_INST1(OP_PUSH8_LIT, T_u8);
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
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
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
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
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
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
        } else if (argc == 1
            && (instruct->args->data.type == ASM_ARG_ADDR
                || instruct->args->data.type == ASM_ARG_LABEL)) {
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
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_ARSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_ARSHIFT_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "sll") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_LLSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_LLSHIFT_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "slr") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_LRSHIFT_LIT, T_u8, T_u8);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_LRSHIFT_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "sub") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_SUB_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_SUB_REG_REG, T_u8, T_u8);
        }  else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)
            && (instruct->args->next->next->data.type == ASM_ARG_ADDR || instruct->args->next->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST3(OP_SUB_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        }else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "subf32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_SUBF32_REG_LIT, T_u8, T_f32);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_SUBF32_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "subf64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_SUBF64_REG_LIT, T_u8, T_f64);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_SUBF64_REG_REG, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR_REG_LIT, T_u8, WORD_T);
        } else if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_REG) {
            DECODE_INST2(OP_XOR_REG_REG, T_u8, T_u8);
        } else if (argc == 3 && instruct->args->data.type == ASM_ARG_LIT
            && (instruct->args->next->data.type == ASM_ARG_ADDR || instruct->args->next->data.type == ASM_ARG_LABEL)
            && (instruct->args->next->next->data.type == ASM_ARG_ADDR || instruct->args->next->next->data.type == ASM_ARG_LABEL)) {
            DECODE_INST3(OP_XOR_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor8") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR8_REG_LIT, T_u8, T_u8);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor16") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR16_REG_LIT, T_u8, T_u16);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor32") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR32_REG_LIT, T_u8, T_u32);
        } else
            return ASM_ERR_ARGS;
    } else if (strcmp(instruct->mnemonic, "xor64") == 0) {
        if (argc == 2 && instruct->args->data.type == ASM_ARG_REG
            && instruct->args->next->data.type == ASM_ARG_LIT) {
            DECODE_INST2(OP_XOR64_REG_LIT, T_u8, T_u64);
        } else
            return ASM_ERR_ARGS;
    } else
        return ASM_ERR_MNEMONIC;
    return ASM_ERR_NONE;
}

int asm_write_instruction(void* buf, unsigned long long offset, struct AsmInstruction* instruct)
{
    switch (instruct->opcode) {
    case OP_HALT:
        BUF_WRITEK(offset, OPCODE_T, OP_HALT);
        break;
    case OP_NOP:
        BUF_WRITEK(offset, OPCODE_T, OP_NOP);
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
    case OP_ADD_MEM_LIT:
        WRITE_INST3(OP_ADD_MEM_LIT, T_u8, UWORD_T, T_u8);
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
        WRITE_INST2(OP_MOV8_MEM_REG, UWORD_T, T_u8);
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
        WRITE_INST2(OP_MOV16_MEM_REG, UWORD_T, T_u8);
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
        WRITE_INST2(OP_MOV32_MEM_REG, UWORD_T, T_u8);
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
        WRITE_INST2(OP_MOV64_MEM_REG, UWORD_T, T_u8);
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
    case OP_PRINT_BIN_REG:
        WRITE_INST1(OP_PRINT_BIN_REG, T_u8);
        break;
    case OP_PRINT_BIN_MEM:
        WRITE_INST2(OP_PRINT_BIN_MEM, T_u8, UWORD_T);
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
        BUF_WRITEK(offset, OPCODE_T, OP_PSTACK);
        break;
    case OP_PREG:
        BUF_WRITEK(offset, OPCODE_T, OP_PREG);
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
        BUF_WRITEK(offset, OPCODE_T, OP_RET);
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
    case OP_SUB_MEM_MEM:
        WRITE_INST3(OP_SUB_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        break;
    case OP_XOR_REG_LIT:
        WRITE_INST2(OP_XOR_REG_LIT, T_u8, WORD_T);
        break;
    case OP_XOR_REG_REG:
        WRITE_INST2(OP_XOR_REG_REG, T_u8, T_u8);
        break;
    case OP_XOR_MEM_MEM:
        WRITE_INST3(OP_XOR_MEM_MEM, T_u8, UWORD_T, UWORD_T);
        break;
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