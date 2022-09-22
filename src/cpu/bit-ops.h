#ifndef __BIT_OPS__H_
#define __BIT_OPS__H_

#include "../util.h"

/** Print seqence of bytes e.g. for `int` call print_bytes(&var, sizeof(int)) to
 * print `XX XX XX XX ` */
void print_bytes(const void *data, const unsigned int length);

/** Print seqence of characters */
void print_chars(const char *data, const unsigned int chars);

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

#endif