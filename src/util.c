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
    if (IS_CHAR(string[length - 1])) --length;
    return strtoll(string, (char **)(string + length), radix);
}