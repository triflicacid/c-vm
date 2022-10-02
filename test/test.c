#include <stdio.h>

#include "../src/util.c"

int main() {
    char string[] = "\\d123";
    char *ptr = string + 1;
    printf("Ptr = %p\n", ptr);
    long long c = decode_escape_seq(&ptr);
    printf("Ptr = %p\n", ptr);
    printf("\"%s\" -> %c (%lli)\n", string, (char)c, c);

    return 0;
}