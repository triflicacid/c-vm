#ifndef __UTIL_H_
#define __UTIL_H_

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

#define IS_BIG_ENDIAN \
    (!(union {        \
          T_u16 u16;  \
          T_u8 c;     \
      }){.u16 = 1}    \
          .c)

#define IS_CHAR(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

#define IS_DIGIT(n) (n >= '0' && n <= '9')

// Macro - easy buffer write. Requires variable `void *buf`. `offset` is
// incremented by `sizeof(type)`
#define BUF_WRITE(offset, type, value)             \
    {                                              \
        (*(type *)((char *)buf + offset) = value); \
        (offset += sizeof(type));                  \
    }

// Macro - easy buffer write. Requires variable `void *buf`. `offset` is
// incremented by `sizeof(type * bytes)`. `ptr` points to start of block to move
// (`void*`)
#define BUF_WRITE_BYTES(offset, ptr, bytes) \
    for (T_u8 i = 0; i < bytes; ++i) BUF_WRITE(offset, T_u8, *((T_u8 *)ptr + i))

/** String to `long long` */
T_i64 str_to_int(const char *string, int length);

#endif