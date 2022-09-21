#include "fetch-exec.h"

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
        case OP_NOT:
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
        default:  // Unknown instruction
            cpu->err = ERR_UNINST;
            cpu->err_data = opcode;
            return 0;
    }
}

int cpu_exec(struct CPU *cpu) {
    WORD_T ip = cpu->regs[REG_IP];
    OPCODE_T instruct = *(OPCODE_T *)((char *)cpu->mem + ip);
    printf("IP = " WORD_T_FLAG "; instruct = " OPCODE_T_FLAG "\n", ip,
           instruct);
    ip += sizeof(OPCODE_T);
    int cnt = cpu_mem_exec(cpu, instruct, &ip);
    if (cpu->err != ERR_NONE) return 0;  // If error, DO NOT continue
    cpu->regs[REG_IP] = ip;
    return cnt;
}

unsigned int cpu_fecycle(struct CPU *cpu) {
    if (cpu->err) return 0;  // Must be error-clear
    unsigned int cnt = 1, i = 0;
    while (cnt && cpu->err == 0) {
        cnt = cpu_exec(cpu);
        i++;
    }
    printf("Process finished with code %i after %i cycles.\n", cpu->err, i);
    if (cpu->err != 0) {
        printf("\n");
        err_print(cpu);
        ERR_CLEAR();
        printf("\n");
    }
    return i;
}