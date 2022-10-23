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

// Is whitespace?
#define IS_WHITESPACE(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')

// Consume whitespace
#define CONSUME_WHITESPACE(string, idx) \
    while (IS_WHITESPACE(string[idx])) ++(idx);

// Is [A-Za-z]
#define IS_CHAR(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

// Is [0-9]
#define IS_DIGIT(n) (n >= '0' && n <= '9')

// Is an octal character?
#define IS_OCTAL(n) (n >= '0' && n <= '8')

// Is a hexadecimal character?
#define IS_HEX(n) \
    ((n >= '0' && n <= '9') || (n >= 'A' && n <= 'F') || (n >= 'a' && n <= 'f'))

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

// Set variable = value if value > variable
#define SET_IF_LARGER(variable, value) \
    if (variable < value) variable = value;

// Is char "c" a valid character for a number of base "base"
#define IS_BASE_CHAR(c, base)                              \
    ((c >= '0' && c <= '0' + (base > 9 ? 9 : base)) ||     \
     (base > 10 ? ((c >= 'a' && c <= 'a' + (base - 10)) || \
                   (c >= 'A' && c <= 'A' + (base - 10)))   \
                : 0))

// Given char "c", return numeric representation. Return 0 otherwise.
#define GET_BASE_VAL(c, base) \
    (c >= 'a' ? c - 'a' + 10  \
              : (c >= 'A' ? c - 'A' + 10 : (c >= '0' ? c - '0' : 0)))

#define CONSOLE_RED "\033[0;31m"
#define CONSOLE_GREEN "\033[0;32m"
#define CONSOLE_BLUE "\033[0;34m"
#define CONSOLE_YELLOW "\033[0;33m"
#define CONSOLE_RESET "\033[0m"

/** Print seqence of bytes e.g. for `int` call print_bytes(&var, sizeof(int)) to
 * print `XX XX XX XX ` */
void print_bytes(const void *data, const unsigned int length);

/** Print seqence of characters */
void print_chars(const char *data, const unsigned int chars);

/** Given a radix suffix, return integer radix. Return -1 ifinvalid radix. */
int get_radix(char suffix);

/** Given a string input containing a number of base "radix", return index where
 * number ends. */
unsigned int scan_number(const char *string, int radix);

/** Integer Base to double */
unsigned long long base_to_10(const char *string, int radix);

/** Float Base to double */
double fbase_to_10(const char *string, int radix);

/** Decode escape sequence '\...'. Pass in pointer. Return character extracted
 * (or -1), and adjust pointer. */
long long decode_escape_seq(char **ptr);

/** Byte array to integer (little-endian) */
T_u64 bytes_to_int(char *ptr, int len);

/** Get substring. REMEMBER to free() the returned pointer. */
char *extract_string(const char *string, unsigned int start, unsigned int len);

#endif