#include "util.h"

#include <stdio.h>

void print_bytes(const void *data, const unsigned int len) {
    for (unsigned int off = 0; off < len; ++off) {
        printf("%.2X ", *((unsigned char *)data + off));
    }
}