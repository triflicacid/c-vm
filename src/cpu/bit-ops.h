#ifndef __BIT_OPS__H_
#define __BIT_OPS__H_

#include "../util.h"

/** Bitwise NOT a sequence of bytes in place */
void bitwise_not(void *data, T_u8 bytes);

/** Bitwise AND two sequences of bytes, place into third buffer */
void bitwise_and(void *b1, void *b2, void *b3, T_u8 bytes);

/** Bitwise OR two sequences of bytes, place into third buffer */
void bitwise_or(void *b1, void *b2, void *b3, T_u8 bytes);

/** Bitwise XOR two sequences of bytes, place into third buffer */
void bitwise_xor(void *b1, void *b2, void *b3, T_u8 bytes);

#endif