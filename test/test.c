#include <stdio.h>

#include "../src/util.c"

int main() {
    const char string[] = "101.1";
    int radix = 2;

    unsigned int iend = scan_number(string, radix);
    printf("IEnd = %u\n", iend);

    double value = base_to_10(string, radix);
    printf("\"%s\" base %i -> %lf base 10\n", string, radix, value);
}