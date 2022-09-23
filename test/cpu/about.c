#include <stdio.h>

#include "_include.c"

int main() {
    struct CPU cpu = cpu_create(128);
    cpu_print_details(&cpu);
    printf("\nREGISTERS:\n");
    cpu_reg_print(&cpu);
    cpu_destroy(&cpu);
}