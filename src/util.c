#include "util.h"

#include <stdlib.h>

T_i64 str_to_int(const char *string, int length) {
    int radix;
    switch (string[length - 1]) {
        case 'h':
            radix = 16;
            break;
        case 'o':
        case 'q':
            radix = 8;
            break;
        case 'b':
        case 'y':
            radix = 2;
            break;
        case 'd':
        case 't':
        default:
            radix = 10;
            break;
    }
    // if (IS_CHAR(string[length - 1])) --length;
    return strtoll(string, (char **)0, radix);
}

long long decode_escape_seq(char **ptr) {
    switch (**ptr) {
        case 'b':  // BACKSPACE
            return 0x8;
        case 'n':  // NEWLINE
            return 0xA;
        case 'r':  // CARRIAGE RETURN
            return 0xD;
        case 't':  // HORIZONTAL TAB
            return 0x9;
        case 'v':  // VERTICAL TAB
            return 0xB;
        case '0':  // NULL
            return 0x0;
        case 's':  // SPACE
            return 0x20;
        case 'd': {  // DECIMAL SEQUENCE
            unsigned int len = 0;
            long long k = 1, value = 0;
            ++(*ptr);
            while (IS_DIGIT(*(*ptr + len))) {
                ++len;
                k *= 10;
            }
            k /= 10;
            for (int i = 0; i < len; ++i, ++(*ptr), k /= 10)
                value += (**ptr - '0') * k;
            return value;
        }
        case 'o': {  // OCTAL SEQUENCE
            unsigned int len = 0;
            long long k = 1, value = 0;
            ++(*ptr);
            while (IS_OCTAL(*(*ptr + len))) {
                ++len;
                k *= 8;
            }
            k /= 8;
            for (int i = 0; i < len; ++i, ++(*ptr), k /= 8)
                value += (**ptr - '0') * k;
            return value;
        }
        case 'x': {  // HEXADECIMAL SEQUENCE
            unsigned int len = 0;
            long long k = 1, value = 0;
            ++(*ptr);
            while (IS_HEX(*(*ptr + len))) {
                ++len;
                k *= 16;
            }
            k /= 16;
            for (int i = 0; i < len; ++i, ++(*ptr), k /= 16) {
                char lb;
                if (**ptr >= '0' && **ptr <= '9')
                    lb = '0';
                else if (**ptr >= 'a' && **ptr <= 'f')
                    lb = 'a';
                else if (**ptr >= 'A' && **ptr <= 'F')
                    lb = 'A';
                value += (**ptr - lb) * k;
            }
            return value;
        }
        default:  // Unknown
            return -1;
    }
}

T_u64 bytes_to_int(char *ptr, int len) {
    T_u64 lit = ptr[0];
    for (int j = 1; j < len; ++j) lit = (lit << 8) | ptr[j];
    return lit;
}

/** Get substring */
char *extract_string(const char *string, unsigned int start, unsigned int len) {
    char *buf = (char *)malloc(len + 1);
    for (unsigned int i = 0; i < len; i++) buf[i] = string[start + i];
    buf[len] = '\0';
    return buf;
}