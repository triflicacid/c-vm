#ifndef __ASM_PARSE_DATA_H__
#define __ASM_PARSE_DATA_H__

#include "linked-list.h"
#include "util.h"
#include "err.h"

#define PARSEDATA_FN_SIGNATURE(name)                                     \
    unsigned int parse_data_##name(                                      \
        char *string, struct LL_NODET_NAME(AsmDataItem_##name) * *items, \
        struct AsmError * err)

// Parse data string. Append items to linked list. Return index of string
// reached.
#define PARSEDATA_CREATE_FN(name, type)                                        \
    PARSEDATA_FN_SIGNATURE(name) {                                             \
        unsigned int slen = strlen(string), i = 0;                             \
        while (i < slen) {                                                     \
            while (i < slen && IS_WHITESPACE(string[i] == ' ')) ++i;           \
            if (string[i] == '\'') {                                           \
                if (string[i + 1] == '\\') {                                   \
                    char *ptr = string + i + 2;                                \
                    long long val = decode_escape_seq(&ptr);                   \
                    linked_list_insert_AsmDataItem_##name((type)val, items,    \
                                                          -1);                 \
                    i = ptr - string + 1;                                      \
                } else {                                                       \
                    if (string[i + 2] == '\'') {                               \
                        linked_list_insert_AsmDataItem_##name(                 \
                            (type)string[i + 1], items, -1);                   \
                        i += 3;                                                \
                    } else {                                                   \
                        return i;                                              \
                    }                                                          \
                }                                                              \
            } else if (string[i] == '"') {                                     \
                ++i;                                                           \
                while (i < slen && string[i] != '"') {                         \
                    if (string[i] == '\\') {                                   \
                        char *ptr = string + i + 1;                            \
                        long long val = decode_escape_seq(&ptr);               \
                        linked_list_insert_AsmDataItem_##name((type)val,       \
                                                              items, -1);      \
                        i = ptr - string;                                      \
                    } else {                                                   \
                        linked_list_insert_AsmDataItem_##name((type)string[i], \
                                                              items, -1);      \
                        ++i;                                                   \
                    }                                                          \
                }                                                              \
                if (string[i] != '"') ++i;                                     \
                ++i;                                                           \
            } else {                                                           \
                unsigned int sublen = 0, has_dp = 0, is_neg = 0;               \
                if (string[i] == '-') {                                        \
                    is_neg = 1;                                                \
                    ++i;                                                       \
                } else if (string[i] == '+') {                                 \
                    is_neg = 0;                                                \
                    ++i;                                                       \
                }                                                              \
                while (i + sublen < slen &&                                    \
                       !IS_WHITESPACE(string[i + sublen])) {                   \
                    if (string[i + sublen] == '.') has_dp = 1;                 \
                    ++sublen;                                                  \
                }                                                              \
                char *sub = malloc(sublen + 1);                                \
                memcpy(sub, string + i, sublen);                               \
                sub[sublen] = '\0';                                            \
                int radix = get_radix(sub[sublen - 1]);                        \
                if (has_dp) {                                                  \
                    double val = fbase_to_10(sub, radix == -1 ? 10 : radix);   \
                    if (is_neg) val = -val;                                    \
                    linked_list_insert_AsmDataItem_##name((type)val, items,    \
                                                          -1);                 \
                } else {                                                       \
                    unsigned long long val =                                   \
                        base_to_10(sub, radix == -1 ? 10 : radix);             \
                    if (is_neg) val = -val;                                    \
                    linked_list_insert_AsmDataItem_##name((type)val, items,    \
                                                          -1);                 \
                }                                                              \
                i += sublen;                                                   \
                free(sub);                                                     \
            }                                                                  \
            while (i < slen && IS_WHITESPACE(string[i])) ++i;                  \
            if (string[i] == ',') ++i;                                         \
        }                                                                      \
        return i;                                                              \
    }

#define PARSEDATA_DECL(name, type)             \
    LL_CREATE_NODET(AsmDataItem_##name, type); \
    LL_DECL_FINSERT(AsmDataItem_##name, type); \
    LL_DECL_FPRINT(AsmDataItem_##name, type);  \
    LL_DECL_FDESTROY(AsmDataItem_##name);      \
    LL_DECL_FSIZE(AsmDataItem_##name);         \
    PARSEDATA_FN_SIGNATURE(name);

#define PARSEDATA_CREATE(name, type)              \
    LL_CREATE_FINSERT(AsmDataItem_##name, type);  \
    LL_CREATE_FPRINT(AsmDataItem_##name, type, {  \
        printf("{");                              \
        print_bytes(&(curr->data), sizeof(type)); \
        printf("} ");                             \
    });                                           \
    LL_CREATE_FSIZE(AsmDataItem_##name);          \
    LL_CREATE_FDESTROY(AsmDataItem_##name, );     \
    PARSEDATA_CREATE_FN(name, type);

// Code to extract data from string and insert into `chunk`. Requires `chunk`,
// `string`, `err`.
#define PARSEDATA_EXTRACT(name, datatype)                            \
    /* Extract data into linked list */                              \
    struct LL_NODET_NAME(AsmDataItem_##name) *items = 0;             \
    int errc = parse_data_##name((char *)string + pos, &items, err); \
    /* Set-up chunk */                                               \
    unsigned int size = linked_list_size_AsmDataItem_##name(items);  \
    chunk->type = ASM_CHUNKT_DATA;                                   \
    datatype *buf;                                                   \
    if (size == 0) {                                                 \
        chunk->bytes = sizeof(datatype);                             \
        buf = malloc(sizeof(datatype));                              \
        *buf = 0;                                                    \
    } else {                                                         \
        chunk->bytes = size * sizeof(datatype);                      \
        /* Transfer into buffer */                                   \
        buf = malloc(chunk->bytes);                                  \
        struct LL_NODET_NAME(AsmDataItem_##name) *cursor = items;    \
        unsigned int offset = 0;                                     \
        while (cursor != 0) {                                        \
            buf[offset] = cursor->data;                              \
            ++offset;                                                \
            cursor = cursor->next;                                   \
        }                                                            \
        linked_list_destroy_AsmDataItem_##name(&items);              \
    }                                                                \
    chunk->data = buf;

PARSEDATA_DECL(u8, T_u8)
PARSEDATA_DECL(u16, T_u16)
PARSEDATA_DECL(u32, T_u32)
PARSEDATA_DECL(u64, T_u64)
PARSEDATA_DECL(f32, T_f32)
PARSEDATA_DECL(f64, T_f64)

#endif