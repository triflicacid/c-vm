#include "util.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void print_bytes(const void *data, T_u32 len) {
    for (T_u32 off = 0; off < len; ++off) {
        printf("%.2X ", *((T_u8 *)data + off));
    }
}

void print_bin(const void *data, T_u32 len) {
    for (int i = 0; i < len; ++i) {
        T_u8 byte = *((T_u8 *)data + i);
        for (int j = 7; j >= 0; --j)
            printf("%d", (byte & (1 << j)) >> j);
        printf(" ");
    }
}

void print_chars(const char *data, T_u32 chars) {
    for (T_u32 off = 0; off < chars; ++off) {
        printf("%c", data[off]);
    }
}

T_u64 bytes_to_int(const char *ptr, int len) {
    T_u64 lit = ptr[0];
    for (int j = 1; j < len; ++j) lit = (lit << 8) | ptr[j];
    return lit;
}

#ifdef __cplusplus
}
#endif
