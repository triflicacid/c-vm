#ifndef __BIT_OPS__H_
#define __BIT_OPS__H_

#define CMP_LT 1
#define CMP_EQ 2
#define CMP_GT 3

#include "../util.h"

/** Bitwise NOT a sequence of bytes in place */
void bitwise_not(void *data, T_u8 bytes);

/** Bitwise AND two sequences of bytes, place into third buffer */
void bitwise_and(void *b1, void *b2, void *b3, T_u8 bytes);

/** Bitwise OR two sequences of bytes, place into third buffer */
void bitwise_or(void *b1, void *b2, void *b3, T_u8 bytes);

/** Bitwise XOR two sequences of bytes, place into third buffer */
void bitwise_xor(void *b1, void *b2, void *b3, T_u8 bytes);

// Add two byte-buffers of byte-length `length` in LITTLE ENDIAN e.g. 0xDEAD ->
// "AD DE". Return overflow.
T_u8 bytes_add(const void *n1, const void *n2, void *nout,
               const unsigned int bytes);

// Like bytes_add, but add a sinle unsigned byte into an n-byte buffer.
T_u8 bytes_add_lit(const void *n1, const T_u8 k, void *nout,
               const unsigned int bytes);

// Compare two byte-buffers of byte-length `length` in LITTLE ENDIAN e.g. 0xDEAD
// -> "AD DE". Return comparison: CMP_EQ, CMP_LT, CMP_GT.
T_u8 bytes_compare(const void *n1, const void *n2, const unsigned int bytes);

#endif