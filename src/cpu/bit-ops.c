#include "bit-ops.h"

void print_bytes(const void *data, const unsigned int len) {
    for (T_u16 off = 0; off < len; ++off) {
        printf("%.2X ", *((T_u8 *)data + off));
    }
}

void print_chars(const char *data, const unsigned int chars) {
    for (T_u16 off = 0; off < chars; ++off) {
        printf("%c", data[off]);
    }
}

void bitwise_not(void *data, T_u8 bytes) {
    for (T_u8 off = 0; off < bytes; ++off) {
        T_u8 *addr = (T_u8 *)data + off;
        *addr = ~*addr;
    }
}

void bitwise_and(void *b1, void *b2, void *b3, T_u8 bytes) {
    for (T_u8 off = 0; off < bytes; ++off) {
        *((T_u8 *)b3 + off) = *((T_u8 *)b1 + off) & *((T_u8 *)b2 + off);
    }
}

void bitwise_or(void *b1, void *b2, void *b3, T_u8 bytes) {
    for (T_u8 off = 0; off < bytes; ++off) {
        *((T_u8 *)b3 + off) = *((T_u8 *)b1 + off) | *((T_u8 *)b2 + off);
    }
}

void bitwise_xor(void *b1, void *b2, void *b3, T_u8 bytes) {
    for (T_u8 off = 0; off < bytes; ++off) {
        *((T_u8 *)b3 + off) = *((T_u8 *)b1 + off) ^ *((T_u8 *)b2 + off);
    }
}

T_u8 bytes_add(const void *n1, const void *n2, void *nout,
               const unsigned int bytes) {
    T_u8 ovfl = 0;
    for (T_u16 off = 0; off < bytes; ++off) {
        T_u16 sum = *((T_u8 *)n1 + off) + *((T_u8 *)n2 + off) + ovfl;
        ovfl = sum >> 8;
        *((T_u8 *)nout + off) = sum & 0xFF;
    }
    return ovfl;
}

T_u8 bytes_compare(const void *n1, const void *n2, const unsigned int bytes) {
    for (T_u16 off = bytes; off > 0; --off) {
        T_u8 a = *((T_u8 *)n1 + off - 1), b = *((T_u8 *)n2 + off - 1);
        if (a == b) continue;
        if (a > b) return CMP_GT;
        return CMP_LT;
    }
    return CMP_EQ;
}