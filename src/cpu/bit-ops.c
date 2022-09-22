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

T_u8 bytes_add(const void *n1, const void *n2, void *nout,
               const unsigned int bytes) {
    T_u8 ovfl = 0;
    for (T_u16 off = 0; off < bytes; ++off) {
        T_u16 sum = *((T_u8 *)n1 + off) + *((T_u8 *)n2 + off) + ovfl;
        ovfl = 0;
        if (sum > 0xFF) {
            ovfl = (sum & 0xFF00) >> 8;
            sum &= 0x00FF;
        }
        *((T_u8 *)nout + off) = sum;
    }
    return ovfl;
}