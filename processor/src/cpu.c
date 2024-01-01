#include "cpu.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include "err.h"
#include "bit-ops.h"

/** Push a new stack frame to the stack */
static void cpu_push_stack_frame(CPU cpu);

/** Pop latest stack frame */
static void cpu_pop_stack_frame(CPU cpu);

struct CPU {
    UWORD_T mem_size;        // Size of .mem
    void *mem;               // Pointer to start of memory block
    WORD_T *regs;  // Register memory
    FILE *out;               // STDOUT
};

CPU cpu_create(WORD_T mem_size) {
    CPU cpu = malloc(sizeof(*cpu));
    cpu->mem_size = mem_size;
    cpu->out = stdout;
    cpu->regs = calloc(REG_COUNT, sizeof(WORD_T));
    cpu->regs[REG_SP] = mem_size;
    cpu->regs[REG_FP] = cpu->regs[REG_SP];
    cpu->mem = calloc(mem_size, 1);
    return cpu;
}

void cpu_set_stack_size(CPU cpu, WORD_T size) {
    cpu->regs[REG_STACK_SIZE] = size;
}

void cpu_set_fout(CPU cpu, FILE *out) {
    cpu->out = out;
}

void cpu_destroy(CPU cpu) {
    free(cpu->mem);
    free(cpu->regs);
}

void cpu_print_details(CPU cpu) {
    WORD_T err = cpu->regs[REG_ERR];

    printf("===== CPU =====\n");
    printf("Memory Size: " WORD_T_FLAG "\n", cpu->mem_size);
    printf("Registers  : %i\n", REG_COUNT);
    printf("Stack Size : %lli\n", cpu->regs[REG_STACK_SIZE]);
    printf("STDOUT     : %i\n", fileno(cpu->out));
    printf("Errno      : 0x%.8llX\n", err);
    if (err != ERR_NONE)
        printf("Error Data : " WORD_T_FLAG "\n", cpu->regs[REG_FLAG]);
    printf("===============\n");
}

void cpu_mem_print(const CPU cpu, WORD_T addr_start, unsigned int length, unsigned char word_size,
                   int per_line) {
    const int max_length = (int) fmax(3, ceil(log2((double) addr_start + length - 1) / 4));

    printf("MEM");

    for (int i = 0; i < max_length; ++i) printf(" ");
    for (int i = 0; i < per_line; ++i) printf("%.*X ", 2 * word_size, i);
    for (int off = 0; off < length; ++off) {
        if (off % per_line == 0) {
            printf("\n%.*llx | ", max_length, addr_start + off);
        }
        WORD_T addr = addr_start + off * word_size;
        for (int k = 0; k < word_size; ++k)
            printf("%.2X", *((unsigned char*)cpu->mem + addr + k));
        printf(" ");
    }

    printf("\n");
}

ERRNO_T cpu_mem_read(CPU cpu, WORD_T addr_start, void* data, unsigned int length) {
    for (int off = 0; off < length; ++off) {
        WORD_T addr = addr_start + off;
        if (addr >= cpu->mem_size) {
            ERR_SET(ERR_MEMOOB, addr)
            return ERR_MEMOOB;
        }
        ((char*)data)[off] = ((char*)cpu->mem)[addr];
    }
    return ERR_NONE;
}

int cpu_load_file_into_mem(CPU cpu, FILE* fp, WORD_T addr_start, size_t length) {
    if (addr_start + length >= cpu->mem_size) {
        ERR_SET(ERR_MEMOOB, cpu->mem_size - 1)
        return 0;
    } else {
        fread((T_u8*)cpu->mem + addr_start, 1, length, fp);
        return 1;
    }
}

ERRNO_T cpu_write_data_into_mem(CPU cpu, WORD_T addr_start, const void *data, unsigned int data_length) {
    for (int off = 0; off < data_length; ++off) {
        WORD_T addr = addr_start + off;
        if (addr >= cpu->mem_size) {
            ERR_SET(ERR_MEMOOB, addr)
            return ERR_MEMOOB;
        }
        ((char*)cpu->mem)[addr] = ((char*)data)[off];
    }
    return ERR_NONE;
}

int cpu_reg_print(CPU cpu) {
    for (int i = 0; i < REG_COUNT; ++i) {
        char str[10];
        switch (i) {
            case REG_ERR:
                sprintf(str, REG_ERR_SYM);
                break;
            case REG_FLAG:
                sprintf(str, REG_FLAG_SYM);
                break;
            case REG_CCR:
                sprintf(str, REG_CCR_SYM);
                break;
            case REG_IP:
                sprintf(str, REG_IP_SYM);
                break;
            case REG_SP:
                sprintf(str, REG_SP_SYM);
                break;
            case REG_FP:
                sprintf(str, REG_FP_SYM);
                break;
            case REG_STACK_SIZE:
                sprintf(str, REG_STACK_SIZE_SYM);
                break;
            default:
                sprintf(str, "r%i", i);
                break;
        }
        printf("%s | ", str);
        if (IS_BIG_ENDIAN)
            for (int j = 0; j < sizeof(WORD_T); --j)
                printf("%.2x", *((T_u8*)cpu->regs + i * sizeof(WORD_T) + j));
        else
            for (int j = sizeof(WORD_T); j > 0; --j)
                printf("%.2x", *((T_u8*)cpu->regs + i * sizeof(WORD_T) + (j - 1)));
        printf("\n");
    }
    return ERR_NONE;
}

void cpu_reg_write(CPU cpu, unsigned int reg_offset, WORD_T value) {
    cpu->regs[reg_offset] = value;
}

WORD_T cpu_reg_read(CPU cpu, unsigned int reg_offset) {
    return cpu->regs[reg_offset];
}

int cpu_save_memory_to_file(CPU cpu, FILE* fp, WORD_T addr_start, size_t length) {
    if (addr_start + length >= cpu->mem_size) {
        ERR_SET(ERR_MEMOOB, cpu->mem_size - 1)
        return 0;
    } else {
        fwrite((T_u8*)cpu->mem + addr_start, 1, length, fp);
        return 1;
    }
}

void cpu_stack_print(CPU cpu) {
    printf("[");
    for (UWORD_T i = 1, addr = cpu->regs[REG_SP]; addr < cpu->mem_size; ++addr, ++i) {
        printf(" %.2X", *((T_u8*)cpu->mem + addr));
        if (i % 20 == 0) printf("\n");
    }
    printf("]\n");
}

void cpu_err_print(CPU cpu) {
    WORD_T err = cpu->regs[REG_ERR];
    if (err == 0) {
        printf("No error\n");
    } else {
        UWORD_T data = cpu->regs[REG_FLAG];
        printf("Error Code: %.8llX\n", err);
        printf("Error Data: " WORD_T_FLAG "\n", data);
        switch (err) {
            case ERR_MEMOOB:
                printf("Attempted to access out-of-bounds memory address 0x%.8llX\n", data);
                break;
            case ERR_REG:
                printf("ERROR: Illegal register offset +%.2llX.\n", data);
                break;
            case ERR_UNINST:
                printf("ERROR: Unknown instruction %.4llX.\n", data);
                break;
            case ERR_STACK_UFLOW:
                printf("ERROR: Stack underflow\n");
                break;
            case ERR_STACK_OFLOW:
                printf("ERROR: Stack overflow - address %.8llX is out-of-bounds (stack lower bound: %.8llX)\n",
                       data, cpu->mem_size - cpu->regs[REG_STACK_SIZE]);
                break;
            default:
                break;
        }
    }
}

int cpu_execute_opcode(CPU cpu, OPCODE_T opcode, WORD_T *ip) {
    // printf("<OPCODE=%X>\n", opcode);
    switch (opcode) {
        case OP_PREG:
            cpu_reg_print(cpu);
            return 1;
        case OP_PMEM:
            cpu_mem_print(cpu, 500, 16, 1, 16);
            return 1;
        case OP_PSTACK:
            cpu_stack_print(cpu);
            return 1;

        case OP_NOP:
            return 1;
        case OP_HALT:
            return 0;
        case OP_MOV_LIT_REG:
        MOV_LIT_REG(*ip, WORD_T)
            return 1;
        case OP_MOV8_LIT_REG:
        MOV_LIT_REG(*ip, T_u8)
            return 1;
        case OP_MOV16_LIT_REG:
        MOV_LIT_REG(*ip, T_u16)
            return 1;
        case OP_MOV32_LIT_REG:
        MOV_LIT_REG(*ip, T_u32)
            return 1;
        case OP_MOV64_LIT_REG:
        MOV_LIT_REG(*ip, T_u64)
            return 1;
        case OP_MOV_LIT_MEM:
        MOV_LIT_MEM(*ip, WORD_T)
            return 1;
        case OP_MOV8_LIT_MEM:
        MOV_LIT_MEM(*ip, T_u8)
            return 1;
        case OP_MOV16_LIT_MEM:
        MOV_LIT_MEM(*ip, T_u16)
            return 1;
        case OP_MOV32_LIT_MEM:
        MOV_LIT_MEM(*ip, T_u32)
            return 1;
        case OP_MOV64_LIT_MEM:
        MOV_LIT_MEM(*ip, T_u64)
            return 1;
        case OP_MOVN_LIT_MEM:
        MOVN_LIT_MEM(*ip)
            return 1;
        case OP_MOV_MEM_REG:
        MOV_MEM_REG(*ip, WORD_T)
            return 1;
        case OP_MOV8_MEM_REG:
        MOV_MEM_REG(*ip, T_u8)
            return 1;
        case OP_MOV16_MEM_REG:
        MOV_MEM_REG(*ip, T_u16)
            return 1;
        case OP_MOV32_MEM_REG:
        MOV_MEM_REG(*ip, T_u32)
            return 1;
        case OP_MOV64_MEM_REG:
        MOV_MEM_REG(*ip, T_u64)
            return 1;
        case OP_MOV_REG_MEM:
        MOV_REG_MEM(*ip, WORD_T)
            return 1;
        case OP_MOV8_REG_MEM:
        MOV_REG_MEM(*ip, T_u8)
            return 1;
        case OP_MOV16_REG_MEM:
        MOV_REG_MEM(*ip, T_u16)
            return 1;
        case OP_MOV32_REG_MEM:
        MOV_REG_MEM(*ip, T_u32)
            return 1;
        case OP_MOV64_REG_MEM:
        MOV_REG_MEM(*ip, T_u64)
            return 1;
        case OP_MOV_REGPTR_REG:
        MOV_REGPTR_REG(*ip, WORD_T)
            return 1;
        case OP_MOV8_REGPTR_REG:
        MOV_REGPTR_REG(*ip, T_u8)
            return 1;
        case OP_MOV16_REGPTR_REG:
        MOV_REGPTR_REG(*ip, T_u16)
            return 1;
        case OP_MOV32_REGPTR_REG:
        MOV_REGPTR_REG(*ip, T_u32)
            return 1;
        case OP_MOV64_REGPTR_REG:
        MOV_REGPTR_REG(*ip, T_u64)
            return 1;
        case OP_MOV_REG_REGPTR:
        MOV_REG_REGPTR(*ip, WORD_T)
            return 1;
        case OP_MOV8_REG_REGPTR:
        MOV_REG_REGPTR(*ip, T_u8)
            return 1;
        case OP_MOV16_REG_REGPTR:
        MOV_REG_REGPTR(*ip, T_u16)
            return 1;
        case OP_MOV32_REG_REGPTR:
        MOV_REG_REGPTR(*ip, T_u32)
            return 1;
        case OP_MOV64_REG_REGPTR:
        MOV_REG_REGPTR(*ip, T_u64)
            return 1;
        case OP_MOV_REG_REG:
        MOV_REG_REG(*ip)
            return 1;
        case OP_MOV_LIT_OFF_REG:
        MOV_LIT_OFF_REG(*ip, WORD_T)
            return 1;
        case OP_MOV8_LIT_OFF_REG:
        MOV_LIT_OFF_REG(*ip, T_u8)
            return 1;
        case OP_MOV16_LIT_OFF_REG:
        MOV_LIT_OFF_REG(*ip, T_u16)
            return 1;
        case OP_MOV32_LIT_OFF_REG:
        MOV_LIT_OFF_REG(*ip, T_u32)
            return 1;
        case OP_MOV64_LIT_OFF_REG:
        MOV_LIT_OFF_REG(*ip, T_u64)
            return 1;
        case OP_AND_REG_LIT:
        OP_REG_LIT(&, *ip, WORD_T, )
            return 1;
        case OP_AND8_REG_LIT:
        OP_REG_LIT(&, *ip, T_u8, )
            return 1;
        case OP_AND16_REG_LIT:
        OP_REG_LIT(&, *ip, T_u16, )
            return 1;
        case OP_AND32_REG_LIT:
        OP_REG_LIT(&, *ip, T_u32, )
            return 1;
        case OP_AND64_REG_LIT:
        OP_REG_LIT(&, *ip, T_u64, )
            return 1;
        case OP_AND_REG_REG:
        OP_REG_REG(&, *ip, WORD_T, )
            return 1;
        case OP_AND_MEM_MEM:
        OP_APPLYF_MEM_MEM(*ip, bitwise_and, )
            return 1;
        case OP_OR_REG_LIT:
        OP_REG_LIT(|, *ip, WORD_T, )
            return 1;
        case OP_OR8_REG_LIT:
        OP_REG_LIT(|, *ip, T_u8, )
            return 1;
        case OP_OR16_REG_LIT:
        OP_REG_LIT(|, *ip, T_u16, )
            return 1;
        case OP_OR32_REG_LIT:
        OP_REG_LIT(|, *ip, T_u32, )
            return 1;
        case OP_OR64_REG_LIT:
        OP_REG_LIT(|, *ip, T_u64, )
            return 1;
        case OP_OR_REG_REG:
        OP_REG_REG(|, *ip, WORD_T, )
            return 1;
        case OP_OR_MEM_MEM:
        OP_APPLYF_MEM_MEM(*ip, bitwise_or, )
            return 1;
        case OP_XOR_REG_LIT:
        OP_REG_LIT(^, *ip, WORD_T, )
            return 1;
        case OP_XOR8_REG_LIT:
        OP_REG_LIT(^, *ip, T_u8, )
            return 1;
        case OP_XOR16_REG_LIT:
        OP_REG_LIT(^, *ip, T_u16, )
            return 1;
        case OP_XOR32_REG_LIT:
        OP_REG_LIT(^, *ip, T_u32, )
            return 1;
        case OP_XOR64_REG_LIT:
        OP_REG_LIT(^, *ip, T_u64, )
            return 1;
        case OP_XOR_REG_REG:
        OP_REG_REG(^, *ip, T_u64, )
            return 1;
        case OP_XOR_MEM_MEM:
        OP_APPLYF_MEM_MEM(*ip, bitwise_xor, )
            return 1;
        case OP_NOT_REG:
        OP_REG(~, , *ip, WORD_T, )
            return 1;
        case OP_NOT_MEM:
        OP_APPLYF_MEM(*ip, bitwise_not, )
            return 1;
        case OP_NEG:
        OP_REG(-, , *ip, WORD_T, )
            return 1;
        case OP_NEGF32:
        OP_REG(-, , *ip, T_f32, )
            return 1;
        case OP_NEGF64:
        OP_REG(-, , *ip, T_f64, )
            return 1;
        case OP_LRSHIFT_LIT:
        OP_REG_LIT(>>, *ip, T_u8, )
            return 1;
        case OP_LRSHIFT_REG:
        OP_REG_REG(>>, *ip, WORD_T, )
            return 1;
        case OP_ARSHIFT_LIT:
        ARS_LIT(*ip, T_u8, )
            return 1;
        case OP_ARSHIFT_REG:
        ARS_REG(*ip, )
            return 1;
        case OP_LLSHIFT_LIT:
        OP_REG_LIT(<<, *ip, T_u8, )
            return 1;
        case OP_LLSHIFT_REG:
        OP_REG_REG(<<, *ip, WORD_T, )
            return 1;
        case OP_CVT_i8_i16:
        OP_CVT(*ip, T_i8, T_i16)
            return 1;
        case OP_CVT_i16_i8:
        OP_CVT(*ip, T_i16, T_i8)
            return 1;
        case OP_CVT_i16_i32:
        OP_CVT(*ip, T_i16, T_i32)
            return 1;
        case OP_CVT_i32_i16:
        OP_CVT(*ip, T_i32, T_i16)
            return 1;
        case OP_CVT_i32_i64:
        OP_CVT(*ip, T_i32, T_i64)
            return 1;
        case OP_CVT_i64_i32:
        OP_CVT(*ip, T_i64, T_i32)
            return 1;
        case OP_CVT_i32_f32:
        OP_CVT(*ip, T_i32, T_f32)
            return 1;
        case OP_CVT_f32_i32:
        OP_CVT(*ip, T_f32, T_i32)
            return 1;
        case OP_CVT_i64_f64:
        OP_CVT(*ip, T_i64, T_f64)
            return 1;
        case OP_CVT_f64_i64:
        OP_CVT(*ip, T_f64, T_i64)
            return 1;
        case OP_ADD_REG_LIT:
        OP_REG_LIT(+, *ip, WORD_T, )
            return 1;
        case OP_ADD_REG_REG:
        OP_REG_REG(+, *ip, WORD_T, )
            return 1;
        case OP_ADDF32_REG_LIT:
        OP_REG_LIT_TYPE(+, *ip, T_f32)
            return 1;
        case OP_ADDF32_REG_REG:
        OP_REG_REG(+, *ip, T_f32, )
            return 1;
        case OP_ADDF64_REG_LIT:
        OP_REG_LIT_TYPE(+, *ip, T_f64)
            return 1;
        case OP_ADDF64_REG_REG:
        OP_REG_REG(+, *ip, T_f64, )
            return 1;
        case OP_ADD_MEM_MEM: {
            T_u8 cry;
            OP_APPLYF_MEM_MEM_RET(*ip, bytes_add, cry)
            cpu->regs[REG_FLAG] = cry;
            return 1;
        }
        case OP_ADD_MEM_LIT: {
            T_u8 bytes = MEM_READ(*ip, T_u8);
            *ip += sizeof(T_u8);
            UWORD_T addr = MEM_READ(*ip, UWORD_T);
            *ip += sizeof(UWORD_T);
            T_u8 lit = MEM_READ(*ip, UWORD_T);
            *ip += sizeof(T_u8);
            void *buf = (void *)((T_u8 *)cpu->mem + addr);
            T_u8 cry = bytes_add_lit(buf, lit, buf, bytes);
            cpu->regs[REG_FLAG] = cry;
            return 1;
        }
        case OP_SUB_REG_LIT:
        OP_REG_LIT(-, *ip, WORD_T, )
            return 1;
        case OP_SUB_REG_REG:
        OP_REG_REG(-, *ip, WORD_T, )
            return 1;
        case OP_SUBF32_REG_LIT:
        OP_REG_LIT_TYPE(-, *ip, T_f32)
            return 1;
        case OP_SUBF32_REG_REG:
        OP_REG_REG(-, *ip, T_f32, )
            return 1;
        case OP_SUBF64_REG_LIT:
        OP_REG_LIT_TYPE(-, *ip, T_f64)
            return 1;
        case OP_SUBF64_REG_REG:
        OP_REG_REG(-, *ip, T_f64, )
            return 1;
        case OP_SUB_MEM_MEM: {
            T_u8 cry;
            OP_APPLYF_MEM_MEM_RET(*ip, bytes_sub, cry)
            cpu->regs[REG_FLAG] = cry;
            return 1;
        }
        case OP_MUL_REG_LIT:
        OP_REG_LIT(*, *ip, WORD_T, )
            return 1;
        case OP_MUL_REG_REG:
        OP_REG_REG(*, *ip, WORD_T, )
            return 1;
        case OP_MULF32_REG_LIT:
        OP_REG_LIT_TYPE(*, *ip, T_f32)
            return 1;
        case OP_MULF32_REG_REG:
        OP_REG_REG(*, *ip, T_f32, )
            return 1;
        case OP_MULF64_REG_LIT:
        OP_REG_LIT_TYPE(*, *ip, T_f64)
            return 1;
        case OP_MULF64_REG_REG:
        OP_REG_REG(*, *ip, T_f64, )
            return 1;
        case OP_DIV_REG_LIT:
        OP_REG_LIT_REG(/, %, *ip, WORD_T, REG_FLAG)
            return 1;
        case OP_DIVF32_REG_LIT:
        OP_REG_LIT(/, *ip, T_f32, )
            return 1;
        case OP_DIVF64_REG_LIT:
        OP_REG_LIT(/, *ip, T_f64, )
            return 1;
        case OP_DIV_REG_REG:
        OP_REG_REG_REG(/, %, *ip, WORD_T, REG_FLAG)
            return 1;
        case OP_DIVF32_REG_REG:
        OP_REG_REG(/, *ip, T_f32, )
            return 1;
        case OP_DIVF64_REG_REG:
        OP_REG_REG(/, *ip, T_f64, )
            return 1;
        case OP_CMP_REG_REG:
        CMP_REG_REG(*ip, WORD_T)
            return 1;
        case OP_CMPF32_REG_REG:
        CMP_REG_REG(*ip, T_f32)
            return 1;
        case OP_CMPF64_REG_REG:
        CMP_REG_REG(*ip, T_f64)
            return 1;
        case OP_CMP_LIT_LIT:
        CMP_LIT_LIT(*ip, WORD_T)
            return 1;
        case OP_CMP_REG_LIT:
        CMP_REG_LIT(*ip, WORD_T)
            return 1;
        case OP_CMPF32_REG_LIT:
        CMP_REG_LIT(*ip, T_f32)
            return 1;
        case OP_CMPF64_REG_LIT:
        CMP_REG_LIT(*ip, T_f64)
            return 1;
        case OP_CMP_MEM_MEM:
        CMP_MEM_MEM(*ip)
            return 1;
        case OP_JMP_ADDR:
        SET_LIT(*ip, *ip, UWORD_T)
            return 1;
        case OP_JMP_REG:
        SET_REG(*ip, *ip, UWORD_T)
            return 1;
        case OP_JMP_EQ_ADDR:
        JMP_LIT_IF(*ip, ==, CMP_EQ)
            return 1;
        case OP_JMP_EQ_REG:
        JMP_REG_IF(*ip, ==, CMP_EQ)
            return 1;
        case OP_JMP_GT_ADDR:
        JMP_LIT_IF(*ip, ==, CMP_GT)
            return 1;
        case OP_JMP_GT_REG:
        JMP_REG_IF(*ip, ==, CMP_GT)
            return 1;
        case OP_JMP_LT_ADDR:
        JMP_LIT_IF(*ip, ==, CMP_LT)
            return 1;
        case OP_JMP_LT_REG:
        JMP_REG_IF(*ip, ==, CMP_LT)
            return 1;
        case OP_JMP_NEQ_ADDR:
        JMP_LIT_IF(*ip, !=, CMP_EQ)
            return 1;
        case OP_JMP_NEQ_REG:
        JMP_REG_IF(*ip, !=, CMP_EQ)
            return 1;
        case OP_PUSH_LIT:
        PUSH_LIT(*ip, WORD_T)
            return 1;
        case OP_PUSH8_LIT:
        PUSH_LIT(*ip, T_u8)
            return 1;
        case OP_PUSH16_LIT:
        PUSH_LIT(*ip, T_u16)
            return 1;
        case OP_PUSH32_LIT:
        PUSH_LIT(*ip, T_u32)
            return 1;
        case OP_PUSH64_LIT:
        PUSH_LIT(*ip, T_u64)
            return 1;
        case OP_PUSHN_LIT:
        PUSHN_LIT(*ip)
            return 1;
        case OP_PUSH_MEM:
        PUSH_MEM(*ip, WORD_T)
            return 1;
        case OP_PUSH8_MEM:
        PUSH_MEM(*ip, T_u8)
            return 1;
        case OP_PUSH16_MEM:
        PUSH_MEM(*ip, T_u16)
            return 1;
        case OP_PUSH32_MEM:
        PUSH_MEM(*ip, T_u32)
            return 1;
        case OP_PUSH64_MEM:
        PUSH_MEM(*ip, T_u64)
            return 1;
        case OP_PUSHN_MEM:
        PUSHN_MEM(*ip)
            return 1;
        case OP_PUSH_REG:
        PUSH_REG(*ip, WORD_T)
            return 1;
        case OP_PUSH8_REG:
        PUSH_REG(*ip, T_u8)
            return 1;
        case OP_PUSH16_REG:
        PUSH_REG(*ip, T_u16)
            return 1;
        case OP_PUSH32_REG:
        PUSH_REG(*ip, T_u32)
            return 1;
        case OP_PUSH64_REG:
        PUSH_REG(*ip, T_u64)
            return 1;
        case OP_POP_REG:
        POP_REG(*ip, WORD_T)
            return 1;
        case OP_POP8_REG:
        POP_REG(*ip, T_u8)
        case OP_POP16_REG:
        POP_REG(*ip, T_u16)
        case OP_POP32_REG:
        POP_REG(*ip, T_u32)
        case OP_POP64_REG:
        POP_REG(*ip, T_u64)
            return 1;
        case OP_POPN_MEM:
        POPN_MEM(*ip)
            return 1;
        case OP_CALL_LIT: {
            WORD_T lit = MEM_READ(*ip, WORD_T);
            *ip += sizeof(WORD_T);
            cpu_push_stack_frame(cpu);
            *ip = lit;
            return 1;
        }
        case OP_CALL_REG: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            *ip += sizeof(T_u8);
            cpu_push_stack_frame(cpu);
            *ip = cpu->regs[reg];
            return 1;
        }
        case OP_RET:
            cpu_pop_stack_frame(cpu);
            return 1;

        case OP_PRINT_HEX_MEM:
        OP_APPLYF_MEM(*ip, print_bytes, )
            return 1;
        case OP_PRINT_HEX_REG: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            *ip += sizeof(T_u8);
            T_u8 *addr = (T_u8 *)(cpu->regs + reg);
            for (int off = 0; off < sizeof(WORD_T); ++off)
                fprintf(cpu->out, "%.2X ", addr[off]);
            return 1;
        }
        case OP_PRINT_BIN_REG: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            *ip += sizeof(T_u8);
            T_u8 *addr = (T_u8 *)(cpu->regs + reg);
            print_bin(addr, sizeof(T_u64));
            return 1;
        }
        case OP_PRINT_BIN_MEM:
        OP_APPLYF_MEM(*ip, print_bin, )
            return 1;
        case OP_PRINT_CHARS_MEM:
        OP_APPLYF_MEM(*ip, print_chars, )
            return 1;
        case OP_PRINT_CHARS_REG: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            *ip += sizeof(T_u8);
            T_u8 *addr = (T_u8 *)(cpu->regs + reg);
            for (int off = 0; off < sizeof(WORD_T); ++off) {
                T_u8 ch = *(addr + off);
                if (ch == '\0') break;
                fprintf(cpu->out, "%c", ch);
            }
            return 1;
        }
        case OP_PRINT_CHARS_LIT:
        OP_APPLYF_LIT(*ip, print_chars)
            return 1;
        case OP_PRINT_INT_REG:
        PRINT_REG(*ip, T_i64, "%lli")
            return 1;
        case OP_PRINT_UINT_REG:
        PRINT_REG(*ip, T_u64, "%llu")
            return 1;
        case OP_PRINT_DBL_REG:
        PRINT_REG(*ip, T_f64, "%lf")
            return 1;
        case OP_GET_CHAR: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            ERR_CHECK_REG(reg) else {
                *ip += sizeof(T_u8);
                cpu->regs[reg] = getch();
            }
            return 1;
        }
        default:  // Unknown instruction
        ERR_SET(ERR_UNINST, opcode)
            return 0;
    }
}

int cpu_execute(CPU cpu) {
    WORD_T *ip = cpu->regs + REG_IP;
    OPCODE_T instruct = *(OPCODE_T *)((T_u8 *)cpu->mem + *ip);
    *ip += sizeof(OPCODE_T);
    int cnt = cpu_execute_opcode(cpu, instruct, ip);
    if (cpu->regs[REG_ERR] != ERR_NONE) return 0;  // If error, DO NOT continue
    return cnt;
}

unsigned int cpu_fetch_execute_cycle(CPU cpu) {
    WORD_T *err = cpu->regs + REG_ERR;
    if (*err) return 0;  // Must be error-clear
    unsigned int cnt = 1, i = 0;
    while (cnt && *err == 0) {
        cnt = cpu_execute(cpu);
        i++;
    }
    printf("Process finished with code %lli after %i cycles.\n", *err, i);
    if (*err != 0) {
        cpu_err_print(cpu);
        printf("\n");
    }
    return i;
}

static void cpu_push_stack_frame(CPU cpu) {
    WORD_T *err = cpu->regs + REG_ERR;

    // Push general purpose registers
    for (int off = 0; off < REG_RESV; ++off) {
        PUSH(WORD_T, cpu->regs[off])
        if (*err != ERR_NONE) return;
    }

    // Push instruction pointer
    PUSH(UWORD_T, cpu->regs[REG_IP])
    if (*err != ERR_NONE) return;

    // Record stack frame size
    UWORD_T frame_size = cpu->regs[REG_FP] - cpu->regs[REG_SP];
    PUSH(UWORD_T, frame_size + sizeof(UWORD_T))
    if (*err != ERR_NONE) return;

    // Move frame pointer
    cpu->regs[REG_FP] = cpu->regs[REG_SP];
}

static void cpu_pop_stack_frame(CPU cpu) {
    WORD_T *err = cpu->regs + REG_ERR;

    // Get frame size
    WORD_T frame_size;
    POP(UWORD_T, frame_size)
    if (*err != ERR_NONE) return;

    // Pop IP
    WORD_T ip;
    POP(UWORD_T, ip)
    if (*err != ERR_NONE) return;
    cpu->regs[REG_IP] = ip;

    // Pop general purpose registers
    for (T_u8 off = REG_RESV; off > 0; --off) {
        POP(UWORD_T, cpu->regs[off - 1])
        if (*err != ERR_NONE) return;
    }

    cpu->regs[REG_FP] += frame_size;
}
