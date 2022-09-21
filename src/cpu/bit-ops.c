#include "bit-ops.h"

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