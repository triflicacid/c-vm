#include "cpu.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "err.h"

struct CPU cpu_create(UWORD_T mem_size) {
    struct CPU cpu = {.mem_size = mem_size};
    cpu.mem = malloc(mem_size);
    cpu.regs[REG_IP] = 0;  // Clear IP register
    cpu.regs[REG_SP] = mem_size;
    cpu.regs[REG_FP] = cpu.regs[REG_SP];
    return cpu;
}

void cpu_destroy(struct CPU* cpu) {
    // Release memory
    free(cpu->mem);
}

void cpu_print_details(struct CPU* cpu) {
    WORD_T err = cpu->regs[REG_ERR];

    printf("===== CPU =====\n");
    printf("Memory Size: " WORD_T_FLAG "\n", cpu->mem_size);
    printf("Registers  : %i\n", REG_COUNT);
    printf("Stack Size : %i\n", cpu->regs[REG_SSIZE]);
    printf("Errno      : 0x%.8X\n", err);
    if (err != ERR_NONE)
        printf("Error Data : " WORD_T_FLAG "\n", cpu->regs[REG_FLAG]);
    printf("===============\n");
}

void cpu_mem_print(const struct CPU* cpu, const UWORD_T addr_start,
                   const unsigned int length, const unsigned char word_size,
                   const int per_line) {
    const int maxlen = fmax(3, ceil(log2(addr_start + length - 1) / 4));
    printf("MEM");
    for (int i = 0; i < maxlen; ++i) printf(" ");
    for (int i = 0; i < per_line; ++i) printf("%.*X ", 2 * word_size, i);
    for (int off = 0; off < length; ++off) {
        if (off % per_line == 0) {
            printf("\n%.*x | ", maxlen, addr_start + off);
        }
        WORD_T addr = addr_start + off * word_size;
        for (int k = 0; k < word_size; ++k)
            printf("%.2X", *((unsigned char*)cpu->mem + addr + k));
        printf(" ");
    }
    printf("\n");
}

ERRNO_T cpu_mem_read(struct CPU* cpu, UWORD_T addr_start, void* data,
                     unsigned int length) {
    for (int off = 0; off < length; ++off) {
        WORD_T addr = addr_start + off;
        if (addr >= cpu->mem_size) {
            ERR_SET(ERR_MEMOOB, addr);
            return ERR_MEMOOB;
        }
        ((char*)data)[off] = ((char*)cpu->mem)[addr];
    }
    return ERR_NONE;
}

int cpu_mem_fread(struct CPU* cpu, FILE* fp, UWORD_T addr_start,
                  size_t length) {
    if (addr_start + length >= cpu->mem_size) {
        ERR_SET(ERR_MEMOOB, cpu->mem_size - 1);
        return 0;
    } else {
        fread((T_u8*)cpu->mem + addr_start, 1, length, fp);
        return 1;
    }
}

ERRNO_T cpu_mem_write_array(struct CPU* cpu, UWORD_T addr_start,
                            const void* data, unsigned int data_length) {
    for (int off = 0; off < data_length; ++off) {
        WORD_T addr = addr_start + off;
        if (addr >= cpu->mem_size) {
            ERR_SET(ERR_MEMOOB, addr);
            return ERR_MEMOOB;
        }
        ((char*)cpu->mem)[addr] = ((char*)data)[off];
    }
    return ERR_NONE;
}

int cpu_mem_fwrite(struct CPU* cpu, FILE* fp, UWORD_T addr_start,
                   size_t length) {
    if (addr_start + length >= cpu->mem_size) {
        ERR_SET(ERR_MEMOOB, cpu->mem_size - 1);
        return 0;
    } else {
        fwrite((T_u8*)cpu->mem + addr_start, 1, length, fp);
        return 1;
    }
}

ERRNO_T cpu_reg_print(struct CPU* cpu) {
    for (int i = 0; i < REG_COUNT; ++i) {
        char str[10];
        switch (i) {
            case REG_ERR:
                sprintf(str, REG_ERR_SYM);
                break;
            case REG_FLAG:
                sprintf(str, REG_FLAG_SYM);
                break;
            // case REG_CMP:
            //     sprintf(str, REG_CMP_SYM);
            //     break;
            case REG_IP:
                sprintf(str, REG_IP_SYM);
                break;
            case REG_SP:
                sprintf(str, REG_SP_SYM);
                break;
            case REG_FP:
                sprintf(str, REG_FP_SYM);
                break;
            case REG_SSIZE:
                sprintf(str, REG_SSIZE_SYM);
                break;
            case REG_FSIZE:
                sprintf(str, REG_FSIZE_SYM);
                break;
            default:
                sprintf(str, "r%i", i);
                break;
        }
        printf("%s | ", str);
        if (IS_BIG_ENDIAN)
            for (T_u8 j = 0; j < sizeof(WORD_T); --j)
                printf("%.2x", *((T_u8*)cpu->regs + i * sizeof(WORD_T) + j));
        else
            for (T_u8 j = sizeof(WORD_T); j > 0; --j)
                printf("%.2x",
                       *((T_u8*)cpu->regs + i * sizeof(WORD_T) + (j - 1)));
        printf("\n");
    }
    return ERR_NONE;
}

void cpu_reg_write(struct CPU* cpu, unsigned int reg_offset, WORD_T value) {
    cpu->regs[reg_offset] = value;
}

WORD_T cpu_reg_read(struct CPU* cpu, unsigned int reg_offset) {
    return cpu->regs[reg_offset];
}

void cpu_stack_print(struct CPU* cpu) {
    printf("[");
    for (UWORD_T addr = cpu->regs[REG_SP]; addr < cpu->mem_size; ++addr) {
        printf(" %.2X", *((T_u8*)cpu->mem + addr));
    }
    printf("]\n");
}