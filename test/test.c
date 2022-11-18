#include <stdio.h>

#include "../src/util.c"
#include "../src/cpu/bit-ops.c"

int main() {
    char n1[] = {0xFA, 00};
    char n2[] = {0x2F, 00};
    char n3[sizeof(n1)] = {0};

    printf("n1 = {");
    print_bytes(n1, sizeof(n1));
    printf("\b}\n");
    printf("n2 = {");
    print_bytes(n2, sizeof(n2));
    printf("\b}\n");

    T_u8 cry = bytes_sub(n1, n2, n3, sizeof(n1));

    printf("n3 = {");
    print_bytes(n3, sizeof(n3));
    printf("\b}\n");
}