#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

#define T_i8 char
#define T_u8 unsigned char
#define T_i16 short int
#define T_u16 unsigned short int
#define T_i32 int
#define T_u32 unsigned int
#define T_i64 long long int
#define T_u64 unsigned long long int
#define T_f32 float
#define T_f64 double

#define WORD_T T_i64
#define WORD_T_FLAG "%lli"
#define UWORD_T T_u64
#define UWORD_T_FLAG "%llu"

#define IS_BIG_ENDIAN \
    (!(union {        \
          T_u16 u16;  \
          T_u8 c;     \
      }){.u16 = 1}    \
          .c)

// Macro - easy buffer write. Requires variable `void *buf`. `offset` is
// incremented by `sizeof(type)`
#define BUF_WRITE(offset, type, value)             \
    {                                              \
        (*(type *)((char *)buf + offset) = value); \
        (offset += sizeof(type));                  \
    }

// Macro - easy buffer write. Requires variable `void *buf`.
#define BUF_WRITEK(offset, type, value) \
    (*(type *)((char *)buf + offset) = value)

// Macro - easy buffer write. Requires variable `void *buf`. `offset` is
// incremented by `sizeof(type * bytes)`. `ptr` points to start of block to move
// (`void*`)
#define BUF_WRITE_BYTES(offset, ptr, bytes) \
    for (T_u8 i = 0; i < bytes; ++i) BUF_WRITE(offset, T_u8, *((T_u8 *)ptr + i))

#define CONSOLE_RED "\033[0;91m"
#define CONSOLE_RED_BG "\033[0;101m"
#define CONSOLE_GREEN "\033[0;32m"
#define CONSOLE_BLUE "\033[0;34m"
#define CONSOLE_YELLOW "\033[0;33m"
#define CONSOLE_RESET "\033[0m"

/** Print sequence of bytes e.g. for `int` call print_bytes(&var, sizeof(int)) to
 * print `XX XX XX XX ` */
void print_bytes(const void *data, unsigned int length);

/** Print sequence of bytes e.g. for `int` call print_bin(&var, sizeof(int)) to
 * print `BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB` */
void print_bin(const void *data, unsigned int length);

/** Print sequence of characters */
void print_chars(const char *data, unsigned int chars);

/** Byte array to integer (little-endian) */
T_u64 bytes_to_int(const char *ptr, int len);

#endif