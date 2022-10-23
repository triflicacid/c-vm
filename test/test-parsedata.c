#include <stdio.h>
#include <string.h>

#include "../src/assembler/parse_data.c"
#include "../src/cpu/bit-ops.c"
#include "../src/util.c"

int main() {
    const char string[] = "1, 3.14";
    printf("STRING: <<%s>>\n", string);
    struct LLNode_AsmDataItem_f32 *items = 0;
    struct AsmError err;
    err.print = 1;

    unsigned int x = parse_data_f32(string, &items, &err);
    printf("X: %u\n", x);
    linked_list_print_AsmDataItem_f32(items);
    return 0;
}