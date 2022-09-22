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

#endif