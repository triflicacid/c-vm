#include "fetch-exec.h"

#include "bit-ops.h"
#include "cpu.h"
#include "err.h"
#include "opcodes.h"

int cpu_mem_exec(struct CPU *cpu, OPCODE_T opcode, UWORD_T *ip) {
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
            MOV_LIT_REG(*ip, WORD_T);
            return 1;
        case OP_MOV8_LIT_REG:
            MOV_LIT_REG(*ip, T_u8);
            return 1;
        case OP_MOV16_LIT_REG:
            MOV_LIT_REG(*ip, T_u16);
            return 1;
        case OP_MOV32_LIT_REG:
            MOV_LIT_REG(*ip, T_u32);
            return 1;
        case OP_MOV64_LIT_REG:
            MOV_LIT_REG(*ip, T_u64);
            return 1;
        case OP_MOV_LIT_MEM:
            MOV_LIT_MEM(*ip, WORD_T);
            return 1;
        case OP_MOV8_LIT_MEM:
            MOV_LIT_MEM(*ip, T_u8);
            return 1;
        case OP_MOV16_LIT_MEM:
            MOV_LIT_MEM(*ip, T_u16);
            return 1;
        case OP_MOV32_LIT_MEM:
            MOV_LIT_MEM(*ip, T_u32);
            return 1;
        case OP_MOV64_LIT_MEM:
            MOV_LIT_MEM(*ip, T_u64);
            return 1;
        case OP_MOVN_LIT_MEM:
            MOVN_LIT_MEM(*ip);
            return 1;
        case OP_MOV_MEM_REG:
            MOV_MEM_REG(*ip, WORD_T);
            return 1;
        case OP_MOV8_MEM_REG:
            MOV_MEM_REG(*ip, T_u8);
            return 1;
        case OP_MOV16_MEM_REG:
            MOV_MEM_REG(*ip, T_u16);
            return 1;
        case OP_MOV32_MEM_REG:
            MOV_MEM_REG(*ip, T_u32);
            return 1;
        case OP_MOV64_MEM_REG:
            MOV_MEM_REG(*ip, T_u64);
            return 1;
        case OP_MOV_REG_MEM:
            MOV_REG_MEM(*ip, WORD_T);
            return 1;
        case OP_MOV8_REG_MEM:
            MOV_REG_MEM(*ip, T_u8);
            return 1;
        case OP_MOV16_REG_MEM:
            MOV_REG_MEM(*ip, T_u16);
            return 1;
        case OP_MOV32_REG_MEM:
            MOV_REG_MEM(*ip, T_u32);
            return 1;
        case OP_MOV64_REG_MEM:
            MOV_REG_MEM(*ip, T_u64);
            return 1;
        case OP_MOV_REGPTR_REG:
            MOV_REGPTR_REG(*ip, WORD_T);
            return 1;
        case OP_MOV8_REGPTR_REG:
            MOV_REGPTR_REG(*ip, T_u8);
            return 1;
        case OP_MOV16_REGPTR_REG:
            MOV_REGPTR_REG(*ip, T_u16);
            return 1;
        case OP_MOV32_REGPTR_REG:
            MOV_REGPTR_REG(*ip, T_u32);
            return 1;
        case OP_MOV64_REGPTR_REG:
            MOV_REGPTR_REG(*ip, T_u64);
            return 1;
        case OP_MOV_REG_REGPTR:
            MOV_REG_REGPTR(*ip, WORD_T);
            return 1;
        case OP_MOV8_REG_REGPTR:
            MOV_REG_REGPTR(*ip, T_u8);
            return 1;
        case OP_MOV16_REG_REGPTR:
            MOV_REG_REGPTR(*ip, T_u16);
            return 1;
        case OP_MOV32_REG_REGPTR:
            MOV_REG_REGPTR(*ip, T_u32);
            return 1;
        case OP_MOV64_REG_REGPTR:
            MOV_REG_REGPTR(*ip, T_u64);
            return 1;
        case OP_MOV_REG_REG:
            MOV_REG_REG(*ip);
            return 1;
        case OP_MOV_LIT_OFF_REG:
            MOV_LIT_OFF_REG(*ip, WORD_T);
            return 1;
        case OP_MOV8_LIT_OFF_REG:
            MOV_LIT_OFF_REG(*ip, T_u8);
            return 1;
        case OP_MOV16_LIT_OFF_REG:
            MOV_LIT_OFF_REG(*ip, T_u16);
            return 1;
        case OP_MOV32_LIT_OFF_REG:
            MOV_LIT_OFF_REG(*ip, T_u32);
            return 1;
        case OP_MOV64_LIT_OFF_REG:
            MOV_LIT_OFF_REG(*ip, T_u64);
            return 1;
        case OP_AND_REG_LIT:
            OP_REG_LIT(&, *ip, WORD_T);
            return 1;
        case OP_AND8_REG_LIT:
            OP_REG_LIT(&, *ip, T_u8);
            return 1;
        case OP_AND16_REG_LIT:
            OP_REG_LIT(&, *ip, T_u16);
            return 1;
        case OP_AND32_REG_LIT:
            OP_REG_LIT(&, *ip, T_u32);
            return 1;
        case OP_AND64_REG_LIT:
            OP_REG_LIT(&, *ip, T_u64);
            return 1;
        case OP_AND_REG_REG:
            OP_REG_REG(&, *ip, WORD_T);
            return 1;
        case OP_AND_MEM_MEM:
            OP_APPLYF_MEM_MEM(*ip, bitwise_and);
            return 1;
        case OP_OR_REG_LIT:
            OP_REG_LIT(|, *ip, WORD_T);
            return 1;
        case OP_OR8_REG_LIT:
            OP_REG_LIT(|, *ip, T_u8);
            return 1;
        case OP_OR16_REG_LIT:
            OP_REG_LIT(|, *ip, T_u16);
            return 1;
        case OP_OR32_REG_LIT:
            OP_REG_LIT(|, *ip, T_u32);
            return 1;
        case OP_OR64_REG_LIT:
            OP_REG_LIT(|, *ip, T_u64);
            return 1;
        case OP_OR_REG_REG:
            OP_REG_REG(|, *ip, WORD_T);
            return 1;
        case OP_OR_MEM_MEM:
            OP_APPLYF_MEM_MEM(*ip, bitwise_or);
            return 1;
        case OP_XOR_REG_LIT:
            OP_REG_LIT(^, *ip, WORD_T);
            return 1;
        case OP_XOR8_REG_LIT:
            OP_REG_LIT(^, *ip, T_u8);
            return 1;
        case OP_XOR16_REG_LIT:
            OP_REG_LIT(^, *ip, T_u16);
            return 1;
        case OP_XOR32_REG_LIT:
            OP_REG_LIT(^, *ip, T_u32);
            return 1;
        case OP_XOR64_REG_LIT:
            OP_REG_LIT(^, *ip, T_u64);
            return 1;
        case OP_XOR_REG_REG:
            OP_REG_REG(^, *ip, WORD_T);
            return 1;
        case OP_XOR_MEM_MEM:
            OP_APPLYF_MEM_MEM(*ip, bitwise_xor);
            return 1;
        case OP_NOT_REG:
            OP_REG(~, , *ip, WORD_T);
            return 1;
        case OP_NOT_MEM:
            OP_APPLYF_MEM(*ip, bitwise_not);
            return 1;
        case OP_NEG:
            OP_REG(-, , *ip, WORD_T);
            return 1;
        case OP_NEGF32:
            OP_REG(-, , *ip, T_f32);
            return 1;
        case OP_NEGF64:
            OP_REG(-, , *ip, T_f64);
            return 1;
        case OP_LRSHIFT_LIT:
            OP_REG_LIT(>>, *ip, T_u8);
        case OP_LRSHIFT_REG:
            OP_REG_REG(>>, *ip, WORD_T);
        case OP_ARSHIFT_LIT:
            ARS_LIT(*ip, T_u8);
            return 1;
        case OP_ARSHIFT_REG:
            ARS_REG(*ip);
            return 1;
        case OP_LSHIFT_LIT:
            OP_REG_LIT(<<, *ip, T_u8);
            return 1;
        case OP_LSHIFT_REG:
            OP_REG_REG(<<, *ip, WORD_T);
            return 1;
        case OP_CVT_i8_i16:
            OP_CVT(*ip, T_i8, T_i16);
            return 1;
        case OP_CVT_i16_i8:
            OP_CVT(*ip, T_i16, T_i8);
            return 1;
        case OP_CVT_i16_i32:
            OP_CVT(*ip, T_i16, T_i32);
            return 1;
        case OP_CVT_i32_i16:
            OP_CVT(*ip, T_i32, T_i16);
            return 1;
        case OP_CVT_i32_i64:
            OP_CVT(*ip, T_i32, T_i64);
            return 1;
        case OP_CVT_i64_i32:
            OP_CVT(*ip, T_i64, T_i32);
            return 1;
        case OP_CVT_i32_f32:
            OP_CVT(*ip, T_i32, T_f32);
            return 1;
        case OP_CVT_f32_i32:
            OP_CVT(*ip, T_f32, T_i32);
            return 1;
        case OP_CVT_i64_f64:
            OP_CVT(*ip, T_i64, T_f64);
            return 1;
        case OP_CVT_f64_i64:
            OP_CVT(*ip, T_f64, T_i64);
            return 1;
        case OP_ADD_REG_LIT:
            OP_REG_LIT(+, *ip, WORD_T);
            return 1;
        case OP_ADD_REG_REG:
            OP_REG_REG(+, *ip, WORD_T);
            return 1;
        case OP_ADDF32_REG_LIT:
            OP_REG_LIT_TYPE(+, *ip, T_f32);
            return 1;
        case OP_ADDF32_REG_REG:
            OP_REG_REG(+, *ip, T_f32);
            return 1;
        case OP_ADDF64_REG_LIT:
            OP_REG_LIT_TYPE(+, *ip, T_f64);
            return 1;
        case OP_ADDF64_REG_REG:
            OP_REG_REG(+, *ip, T_f64);
            return 1;
        case OP_ADD_MEM_MEM: {
            T_u8 cry;
            OP_APPLYF_MEM_MEM_RET(*ip, bytes_add, cry);
            cpu->regs[REG_FLAG] = cry;
            return 1;
        }
        case OP_SUB_REG_LIT:
            OP_REG_LIT(-, *ip, WORD_T);
            return 1;
        case OP_SUB_REG_REG:
            OP_REG_REG(-, *ip, WORD_T);
            return 1;
        case OP_SUBF32_REG_LIT:
            OP_REG_LIT_TYPE(-, *ip, T_f32);
            return 1;
        case OP_SUBF32_REG_REG:
            OP_REG_REG(-, *ip, T_f32);
            return 1;
        case OP_SUBF64_REG_LIT:
            OP_REG_LIT_TYPE(-, *ip, T_f64);
            return 1;
        case OP_SUBF64_REG_REG:
            OP_REG_REG(-, *ip, T_f64);
            return 1;
        case OP_MUL_REG_LIT:
            OP_REG_LIT(*, *ip, WORD_T);
            return 1;
        case OP_MUL_REG_REG:
            OP_REG_REG(*, *ip, WORD_T);
            return 1;
        case OP_MULF32_REG_LIT:
            OP_REG_LIT_TYPE(*, *ip, T_f32);
            return 1;
        case OP_MULF32_REG_REG:
            OP_REG_REG(*, *ip, T_f32);
            return 1;
        case OP_MULF64_REG_LIT:
            OP_REG_LIT_TYPE(*, *ip, T_f64);
            return 1;
        case OP_MULF64_REG_REG:
            OP_REG_REG(*, *ip, T_f64);
            return 1;
        case OP_DIV_REG_LIT:
            OP_REG_LIT(/, *ip, WORD_T);
            return 1;
        case OP_DIVF32_REG_LIT:
            OP_REG_LIT(/, *ip, T_f32);
            return 1;
        case OP_DIVF64_REG_LIT:
            OP_REG_LIT(/, *ip, T_f64);
            return 1;
        case OP_DIV_REG_REG:
            OP_REG_REG_REG(/, %, *ip, WORD_T, REG_FLAG);
            return 1;
        case OP_DIVF32_REG_REG:
            OP_REG_REG(/, *ip, T_f32);
            return 1;
        case OP_DIVF64_REG_REG:
            OP_REG_REG(/, *ip, T_f64);
            return 1;
        case OP_CMP_REG_REG:
            CMP_REG_REG(*ip, WORD_T);
            return 1;
        case OP_CMPF32_REG_REG:
            CMP_REG_REG(*ip, T_f32);
            return 1;
        case OP_CMPF64_REG_REG:
            CMP_REG_REG(*ip, T_f64);
            return 1;
        case OP_CMP_REG_LIT:
            CMP_REG_LIT(*ip, WORD_T);
            return 1;
        case OP_CMPF32_REG_LIT:
            CMP_REG_LIT(*ip, T_f32);
            return 1;
        case OP_CMPF64_REG_LIT:
            CMP_REG_LIT(*ip, T_f64);
            return 1;
        case OP_CMP_MEM_MEM:
            CMP_MEM_MEM(*ip);
            return 1;
        case OP_JMP_LIT:
            SET_LIT(*ip, *ip, UWORD_T);
            return 1;
        case OP_JMP_REG:
            SET_REG(*ip, *ip, UWORD_T);
            return 1;
        case OP_JMP_EQ_LIT:
            JMP_LIT_IF(*ip, ==, CMP_EQ);
            return 1;
        case OP_JMP_EQ_REG:
            JMP_REG_IF(*ip, ==, CMP_EQ);
            return 1;
        case OP_JMP_GT_LIT:
            JMP_LIT_IF(*ip, ==, CMP_GT);
            return 1;
        case OP_JMP_GT_REG:
            JMP_REG_IF(*ip, ==, CMP_GT);
            return 1;
        case OP_JMP_LT_LIT:
            JMP_LIT_IF(*ip, ==, CMP_LT);
            return 1;
        case OP_JMP_LT_REG:
            JMP_REG_IF(*ip, ==, CMP_LT);
            return 1;
        case OP_JMP_NEQ_LIT:
            JMP_LIT_IF(*ip, !=, CMP_EQ);
            return 1;
        case OP_JMP_NEQ_REG:
            JMP_REG_IF(*ip, !=, CMP_EQ);
            return 1;
        case OP_PUSH_LIT:
            PUSH_LIT(*ip, WORD_T);
            return 1;
        case OP_PUSH8_LIT:
            PUSH_LIT(*ip, T_u8);
            return 1;
        case OP_PUSH16_LIT:
            PUSH_LIT(*ip, T_u16);
            return 1;
        case OP_PUSH32_LIT:
            PUSH_LIT(*ip, T_u32);
            return 1;
        case OP_PUSH64_LIT:
            PUSH_LIT(*ip, T_u64);
            return 1;
        case OP_PUSHN_LIT:
            PUSHN_LIT(*ip);
            return 1;
        case OP_PUSH_MEM:
            PUSH_MEM(*ip, WORD_T);
            return 1;
        case OP_PUSH8_MEM:
            PUSH_MEM(*ip, T_u8);
            return 1;
        case OP_PUSH16_MEM:
            PUSH_MEM(*ip, T_u16);
            return 1;
        case OP_PUSH32_MEM:
            PUSH_MEM(*ip, T_u32);
            return 1;
        case OP_PUSH64_MEM:
            PUSH_MEM(*ip, T_u64);
            return 1;
        case OP_PUSHN_MEM:
            PUSHN_MEM(*ip);
            return 1;
        case OP_PUSH_REG:
            PUSH_REG(*ip, WORD_T);
            return 1;
        case OP_PUSH8_REG:
            PUSH_REG(*ip, T_u8);
            return 1;
        case OP_PUSH16_REG:
            PUSH_REG(*ip, T_u16);
            return 1;
        case OP_PUSH32_REG:
            PUSH_REG(*ip, T_u32);
            return 1;
        case OP_PUSH64_REG:
            PUSH_REG(*ip, T_u64);
            return 1;
        case OP_POP_REG:
            POP_REG(*ip, WORD_T);
            return 1;
        case OP_POP8_REG:
            POP_REG(*ip, T_u8);
        case OP_POP16_REG:
            POP_REG(*ip, T_u16);
        case OP_POP32_REG:
            POP_REG(*ip, T_u32);
        case OP_POP64_REG:
            POP_REG(*ip, T_u64);
            return 1;
        case OP_POPN_MEM:
            POPN_MEM(*ip);
            return 1;
        case OP_CALL_LIT: {
            UWORD_T lit = MEM_READ(*ip, UWORD_T);
            *ip += sizeof(UWORD_T);
            cpu_stackframe_push(cpu);
            *ip = lit;
            return 1;
        }
        case OP_CALL_REG: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            *ip += sizeof(T_u8);
            cpu_stackframe_push(cpu);
            *ip = cpu->regs[reg];
            return 1;
        }
        case OP_RET:
            cpu_stackframe_pop(cpu);
            return 1;

        case OP_PRINT_HEX_MEM:
            OP_APPLYF_MEM(*ip, print_bytes);
            return 1;
        case OP_PRINT_HEX_REG: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            *ip += sizeof(T_u8);
            T_u8 *addr = (T_u8 *)(cpu->regs + reg);
            for (T_u8 off = 0; off < sizeof(WORD_T); ++off)
                printf("%.2X ", addr[off]);
            return 1;
        }
        case OP_PRINT_CHARS_MEM:
            OP_APPLYF_MEM(*ip, print_chars);
            return 1;
        case OP_PRINT_CHARS_REG: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            *ip += sizeof(T_u8);
            T_u8 *addr = (T_u8 *)(cpu->regs + reg);
            for (T_u8 off = 0; off < sizeof(WORD_T); ++off) {
                T_u8 ch = addr[off];
                if (ch == '\0') break;
                printf("%c", ch);
            }
            return 1;
        }
        case OP_PRINT_CHARS_LIT:
            OP_APPLYF_LIT(*ip, print_chars);
            return 1;
        case OP_GET_CHAR: {
            T_u8 reg = MEM_READ(*ip, T_u8);
            ERR_CHECK_REG(reg) else {
                *ip += sizeof(T_u8);
                char c = getch();
                cpu->regs[reg] = c;
            }
            return 1;
        }
        default:  // Unknown instruction
            ERR_SET(ERR_UNINST, opcode);
            return 0;
    }
    return 1;
}

int cpu_exec(struct CPU *cpu) {
    WORD_T *ip = cpu->regs + REG_IP;
    OPCODE_T instruct = *(OPCODE_T *)((T_u8 *)cpu->mem + *ip);
    *ip += sizeof(OPCODE_T);
    int cnt = cpu_mem_exec(cpu, instruct, ip);
    if (cpu->regs[REG_ERR] != ERR_NONE) return 0;  // If error, DO NOT continue
    return cnt;
}

unsigned int cpu_fecycle(struct CPU *cpu) {
    WORD_T *err = cpu->regs + REG_ERR;
    if (*err) return 0;  // Must be error-clear
    unsigned int cnt = 1, i = 0;
    while (cnt && *err == 0) {
        cnt = cpu_exec(cpu);
        i++;
    }
    printf("Process finished with code %i after %i cycles.\n", *err, i);
    if (*err != 0) {
        err_print(cpu);
        printf("\n");
    }
    return i;
}

void cpu_stackframe_push(struct CPU *cpu) {
    UWORD_T *err = cpu->regs + REG_ERR;

    // Push general purpose registers
    for (T_u8 off = 0; off < REG_RESV; ++off) {
        PUSH(WORD_T, cpu->regs[off]);
        if (*err != ERR_NONE) return;
    }

    // Push instruction pointer
    PUSH(UWORD_T, cpu->regs[REG_IP]);
    if (*err != ERR_NONE) return;

    // Record stack frame size
    UWORD_T frame_size = cpu->regs[REG_FP] - cpu->regs[REG_SP];
    PUSH(UWORD_T, frame_size + sizeof(UWORD_T));
    if (*err != ERR_NONE) return;

    // Move frame pointer
    cpu->regs[REG_FP] = cpu->regs[REG_SP];
}

void cpu_stackframe_pop(struct CPU *cpu) {
    UWORD_T *err = cpu->regs + REG_ERR;

    // Get frame size
    UWORD_T frame_size;
    POP(UWORD_T, frame_size);
    if (*err != ERR_NONE) return;

    // Pop IP
    UWORD_T ip;
    POP(UWORD_T, ip);
    if (*err != ERR_NONE) return;
    cpu->regs[REG_IP] = ip;

    // Pop general purpose registers
    for (T_u8 off = REG_RESV; off > 0; --off) {
        POP(UWORD_T, cpu->regs[off - 1]);
        if (*err != ERR_NONE) return;
    }

    cpu->regs[REG_FP] += frame_size;
}