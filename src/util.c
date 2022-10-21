#include "util.h"

#include <stdlib.h>

int get_radix(char suffix) {
    switch (suffix) {
        case 'h':
            return 16;
        case 'o':
        case 'q':
            return 8;
        case 'b':
        case 'y':
            return 2;
        case 'd':
        case 't':
            return 10;
        default:
            return -1;
    }
}

long long decode_escape_seq(char **ptr) {
    switch (**ptr) {
        case 'b':  // BACKSPACE
            ++(*ptr);
            return 0x8;
        case 'n':  // NEWLINE
            ++(*ptr);
            return 0xA;
        case 'r':  // CARRIAGE RETURN
            ++(*ptr);
            return 0xD;
        case 's':  // SPACE
            ++(*ptr);
            return 0x20;
        case 't':  // HORIZONTAL TAB
            ++(*ptr);
            return 0x9;
        case 'v':  // VERTICAL TAB
            ++(*ptr);
            return 0xB;
        case '0':  // NULL
            ++(*ptr);
            return 0x0;
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

unsigned int scan_number(const char *string, int radix) {
    int found_dp = 0;
    unsigned int i = 0;
    while (string[i] != '\0' &&
           (string[i] == '_' || IS_BASE_CHAR(string[i], radix))) {
        ++i;
        if (string[i] == '.' && !found_dp) {
            ++i;
            found_dp = 1;
        }
    }
    return i;
}

unsigned long long base_to_10(const char *string, int radix) {
    unsigned long long value = 0;  // Base 10 value
    unsigned long long k = 1;      // Multiplying factor
    unsigned int i = 0;
    // Calculate integer exponent
    while (string[i] != '\0' &&
           (string[i] == '_' || IS_BASE_CHAR(string[i], radix))) {
        if (string[i] != '_' && i != 0) k *= radix;
        ++i;
    }
    // Calculate number
    i = 0;
    int found_dp = 0;
    while (string[i] != '\0' &&
           (string[i] == '_' || IS_BASE_CHAR(string[i], radix))) {
        if (string[i] != '_') {
            value += GET_BASE_VAL(string[i], radix) * k;
            k /= radix;
        }
        ++i;
    }
    return value;
}

double fbase_to_10(const char *string, int radix) {
    double value = 0;  // Base 10 value
    double k = 1;      // Multiplying factor
    unsigned int i = 0;
    // Calculate integer exponent
    while (string[i] != '\0' &&
           (string[i] == '_' || IS_BASE_CHAR(string[i], radix))) {
        if (string[i] != '_' && i != 0) k *= radix;
        ++i;
    }
    // Calculate number
    i = 0;
    int found_dp = 0;
    while (string[i] != '\0' &&
           (string[i] == '_' || IS_BASE_CHAR(string[i], radix))) {
        if (string[i] != '_') {
            value += GET_BASE_VAL(string[i], radix) * k;
            k /= radix;
        }
        ++i;
        if (string[i] == '.' && !found_dp) {
            ++i;
            found_dp = 1;
        }
    }
    return value;
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