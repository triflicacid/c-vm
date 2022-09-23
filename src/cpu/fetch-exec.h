// DEFINE MACROS FOR IMPLEMENTING CERTAIN INSTRUCTIONS
#ifndef __FETCH_EXEC__H_
#define __FETCH_EXEC__H_

#include "../util.h"
#include "cpu.h"
#include "err.h"

// Set `var` to a literal
#define SET_LIT(ip, var, type)          \
    {                                   \
        type addr = MEM_READ(ip, type); \
        ERR_CHECK_ADDR(addr) else {     \
            ip += sizeof(type);         \
            var = addr;                 \
        }                               \
    }

// Set `var` to a register value
#define SET_REG(ip, var, type)                \
    {                                         \
        T_u8 reg = MEM_READ(ip, T_u8);        \
        ERR_CHECK_REG(reg) else {             \
            ip += sizeof(T_u8);               \
            var = *(type *)(cpu->regs + reg); \
        }                                     \
    }

// Move literal of type `type` at `ip` to register at `ip+1`
#define MOV_LIT_REG(ip, type)           \
    {                                   \
        type data = MEM_READ(ip, type); \
        ip += sizeof(type);             \
        T_u8 reg = MEM_READ(ip, T_u8);  \
        ERR_CHECK_REG(reg) else {       \
            ip += sizeof(T_u8);         \
            cpu->regs[reg] = data;      \
        }                               \
    }

// Move literal of type `type` at `ip` to memory address at `ip+1`
#define MOV_LIT_MEM(ip, type)                 \
    {                                         \
        type data = MEM_READ(ip, type);       \
        ip += sizeof(type);                   \
        UWORD_T addr = MEM_READ(ip, UWORD_T); \
        ip += sizeof(UWORD_T);                \
        MEM_WRITEK(addr, type, data);         \
    }

// Move n-bytes at `ip+2` to memory address at `ip+1`
#define MOVN_LIT_MEM(ip)                                      \
    {                                                         \
        T_u8 nbytes = MEM_READ(ip, T_u8);                     \
        ip += sizeof(nbytes);                                 \
        UWORD_T addr = MEM_READ(ip, UWORD_T);                 \
        ip += sizeof(UWORD_T);                                \
        for (UWORD_T off = 0; off < nbytes; ++off) {          \
            if (addr + off >= cpu->mem_size) {                \
                ERR_SET(ERR_MEMOOB, addr + off);              \
                break;                                        \
            }                                                 \
            MEM_WRITEK(addr + off, T_u8, MEM_READ(ip, T_u8)); \
            ip += sizeof(T_u8);                               \
        }                                                     \
    }

// Move value of type `type` at memory address at `ip` to register offset at
// `ip+1`
#define MOV_MEM_REG(ip, type)                 \
    {                                         \
        UWORD_T addr = MEM_READ(ip, UWORD_T); \
        ip += sizeof(UWORD_T);                \
        T_u8 reg = MEM_READ(ip, T_u8);        \
        ERR_CHECK_REG(reg) else {             \
            ip += sizeof(T_u8);               \
            type data = MEM_READ(addr, type); \
            cpu->regs[reg] = data;            \
        }                                     \
    }

// Move value of type `type` from register at `ip` to memory address at `ip+1`
#define MOV_REG_MEM(ip, type)                       \
    {                                               \
        T_u8 reg = MEM_READ(ip, T_u8);              \
        ERR_CHECK_REG(reg) else {                   \
            ip += sizeof(T_u8);                     \
            UWORD_T addr = MEM_READ(ip, UWORD_T);   \
            ip += sizeof(UWORD_T);                  \
            type data = *(type *)(cpu->regs + reg); \
            MEM_WRITEK(addr, type, data);           \
        }                                           \
    }

// Move value of from register `ip` to register `ip+1`
#define MOV_REG_REG(ip)                            \
    {                                              \
        T_u8 regA = MEM_READ(ip, T_u8);            \
        ERR_CHECK_REG(regA) else {                 \
            ip += sizeof(T_u8);                    \
            T_u8 regB = MEM_READ(ip, T_u8);        \
            ERR_CHECK_REG(regB) else {             \
                ip += sizeof(T_u8);                \
                cpu->regs[regB] = cpu->regs[regA]; \
            }                                      \
        }                                          \
    }

// Move value of type `type` from address stored in register at `ip` to register
// at `ip+1`
#define MOV_REGPTR_REG(ip, type)                  \
    {                                             \
        T_u8 regA = MEM_READ(ip, T_u8);           \
        ERR_CHECK_REG(regA) else {                \
            ip += sizeof(T_u8);                   \
            T_u8 regB = MEM_READ(ip, T_u8);       \
            ERR_CHECK_REG(regB) else {            \
                ip += sizeof(T_u8);               \
                UWORD_T addr = cpu->regs[regA];   \
                type data = MEM_READ(addr, type); \
                cpu->regs[regB] = data;           \
            }                                     \
        }                                         \
    }

// Move value of type `type` from register `ip` to memory address stored in
// register `ip+1`
#define MOV_REG_REGPTR(ip, type)                \
    {                                           \
        T_u8 regA = MEM_READ(ip, T_u8);         \
        ERR_CHECK_REG(regA) else {              \
            ip += sizeof(T_u8);                 \
            T_u8 regB = MEM_READ(ip, T_u8);     \
            ERR_CHECK_REG(regB) else {          \
                ip += sizeof(T_u8);             \
                type data = cpu->regs[regA];    \
                UWORD_T addr = cpu->regs[regB]; \
                MEM_WRITEK(addr, type, data);   \
            }                                   \
        }                                       \
    }

// Move value of type `type` from [register + memory address] to another
// register
#define MOV_LIT_OFF_REG(ip, type)                                 \
    {                                                             \
        UWORD_T addr = MEM_READ(ip, UWORD_T);                     \
        ip += sizeof(UWORD_T);                                    \
        T_u8 r1 = MEM_READ(ip, T_u8);                             \
        ERR_CHECK_REG(r1) {                                       \
            ip += sizeof(T_u8);                                   \
            T_u8 r2 = MEM_READ(ip, T_u8);                         \
            ERR_CHECK_REG(r2) else {                              \
                ip += sizeof(T_u8);                               \
                type data = MEM_READ(addr + cpu->regs[r1], type); \
                cpu->regs[r2] = data;                             \
            }                                                     \
        }                                                         \
    }

// Perform operation between register and literal : reg = reg op lit
#define OP_REG_LIT(op, ip, litT)                    \
    {                                               \
        T_u8 reg = MEM_READ(ip, T_u8);              \
        ERR_CHECK_REG(reg) else {                   \
            ip += sizeof(T_u8);                     \
            litT lit = MEM_READ(ip, litT);          \
            ip += sizeof(litT);                     \
            cpu->regs[reg] = cpu->regs[reg] op lit; \
        }                                           \
    }

// Perform operation between register and literal : reg = reg op lit. Coerce
// `reg` and `lit` to type `type`
#define OP_REG_LIT_TYPE(op, ip, type)                   \
    {                                                   \
        T_u8 reg = MEM_READ(ip, T_u8);                  \
        ERR_CHECK_REG(reg) else {                       \
            ip += sizeof(T_u8);                         \
            type lit = MEM_READ(ip, type);              \
            ip += sizeof(type);                         \
            type v = *(type *)(cpu->regs + reg) op lit; \
            cpu->regs[reg] = *(WORD_T *)&v;             \
        }                                               \
    }

// Perform operation between 2 registers in type `type` : r1 = r1 op r2
#define OP_REG_REG(op, ip, type)                                             \
    {                                                                        \
        T_u8 r1 = MEM_READ(ip, T_u8);                                        \
        ERR_CHECK_REG(r1) else {                                             \
            ip += sizeof(T_u8);                                              \
            T_u8 r2 = MEM_READ(ip, T_u8);                                    \
            ERR_CHECK_REG(r2) else {                                         \
                ip += sizeof(T_u8);                                          \
                type v =                                                     \
                    *(type *)(cpu->regs + r1) op * (type *)(cpu->regs + r2); \
                cpu->regs[r1] = *(WORD_T *)&v;                               \
            }                                                                \
        }                                                                    \
    }

// Perform operation on one register : reg = pre reg post
#define OP_REG(pre, post, ip, type)                        \
    {                                                      \
        T_u8 reg = MEM_READ(ip, T_u8);                     \
        ERR_CHECK_REG(reg) else {                          \
            ip += sizeof(T_u8);                            \
            type v = pre * (type *)(cpu->regs + reg) post; \
            cpu->regs[reg] = *(WORD_T *)&v;                \
        }                                                  \
    }

// Perform operation between 2 registers in type `type` : r1 = r1 op1 r2, r3
// = r1 op2 r2
#define OP_REG_REG_REG(op1, op2, ip, type, r3)        \
    {                                                 \
        T_u8 r1 = MEM_READ(ip, T_u8);                 \
        ERR_CHECK_REG(r1) else {                      \
            ip += sizeof(T_u8);                       \
            T_u8 r2 = MEM_READ(ip, T_u8);             \
            ERR_CHECK_REG(r2) else {                  \
                ip += sizeof(T_u8);                   \
                type v1 = (*(type *)(cpu->regs + r1)) \
                    op1(*(type *)(cpu->regs + r2));   \
                type v2 = (*(type *)(cpu->regs + r1)) \
                    op2(*(type *)(cpu->regs + r2));   \
                cpu->regs[r1] = *(WORD_T *)&v1;       \
                cpu->regs[r3] = *(WORD_T *)&v2;       \
            }                                         \
        }                                             \
    }

#define ARS(x, n) ((x < 0 && n > 0) ? (x >> n | ~(~0U >> n)) : (x >> n))

// Perform arithmetic right shift : r1 = r1 a>> r2
#define ARS_REG(ip)                                                \
    {                                                              \
        T_u8 r1 = MEM_READ(ip, T_u8);                              \
        ERR_CHECK_REG(r1) else {                                   \
            ip += sizeof(T_u8);                                    \
            T_u8 r2 = MEM_READ(ip, T_u8);                          \
            ERR_CHECK_REG(r2) else {                               \
                ip += sizeof(T_u8);                                \
                cpu->regs[r1] = ARS(cpu->regs[r1], cpu->regs[r2]); \
            }                                                      \
        }                                                          \
    }

// Perform arithmetic right shift : reg = reg a>> lit
#define ARS_LIT(ip, type)                              \
    {                                                  \
        T_u8 reg = MEM_READ(ip, T_u8);                 \
        ERR_CHECK_REG(reg) else {                      \
            ip += sizeof(T_u8);                        \
            type lit = MEM_READ(ip, type);             \
            ip += sizeof(type);                        \
            cpu->regs[reg] = ARS(cpu->regs[reg], lit); \
        }                                              \
    }

// Convert register form one data-type to another. dt1 -> dt2
#define OP_CVT(ip, dt1, dt2)                                        \
    {                                                               \
        T_u8 reg = MEM_READ(ip, T_u8);                              \
        ERR_CHECK_REG(reg) else {                                   \
            ip += sizeof(T_u8);                                     \
            dt2 v = (dt2)((dt1)cpu->regs[reg]);                     \
            *(dt2 *)((char *)cpu->regs + reg * sizeof(WORD_T)) = v; \
        }                                                           \
    }

// Instruction syntax `<bytes: u8> <addr: uword>`. In-place modify `addr` result
// of `fname(addr, bytes)`
#define OP_APPLYF_MEM(ip, fname)                         \
    {                                                    \
        T_u8 bytes = MEM_READ(ip, T_u8);                 \
        ip += sizeof(T_u8);                              \
        UWORD_T addr = MEM_READ(ip, UWORD_T);            \
        ip += sizeof(UWORD_T);                           \
        fname((void *)((T_u8 *)cpu->mem + addr), bytes); \
    }

// Instruction syntax `<bytes: u8> <addr1: uword> <addr2: uword>`. In-place
// modify `addr1` result of `fname(addr1, addr2, addr1, bytes)`. Set `retVar` to
// return value of `fname`.
#define OP_APPLYF_MEM_MEM(ip, fname)                     \
    {                                                    \
        T_u8 bytes = MEM_READ(ip, T_u8);                 \
        ip += sizeof(T_u8);                              \
        UWORD_T addr1 = MEM_READ(ip, UWORD_T);           \
        ip += sizeof(UWORD_T);                           \
        UWORD_T addr2 = MEM_READ(ip, UWORD_T);           \
        ip += sizeof(UWORD_T);                           \
        void *buf1 = (void *)((T_u8 *)cpu->mem + addr1); \
        void *buf2 = (void *)((T_u8 *)cpu->mem + addr2); \
        fname(buf1, buf2, buf1, bytes);                  \
    }

// Instruction syntax `<bytes: u8> <addr1: uword> <addr2: uword>`. In-place
// modify `addr1` result of `fname(addr1, addr2, addr1, bytes)`. Set `retVar` to
// return value of `fname`.
#define OP_APPLYF_MEM_MEM_RET(ip, fname, retVar)         \
    {                                                    \
        T_u8 bytes = MEM_READ(ip, T_u8);                 \
        ip += sizeof(T_u8);                              \
        UWORD_T addr1 = MEM_READ(ip, UWORD_T);           \
        ip += sizeof(UWORD_T);                           \
        UWORD_T addr2 = MEM_READ(ip, UWORD_T);           \
        ip += sizeof(UWORD_T);                           \
        void *buf1 = (void *)((T_u8 *)cpu->mem + addr1); \
        void *buf2 = (void *)((T_u8 *)cpu->mem + addr2); \
        retVar = fname(buf1, buf2, buf1, bytes);         \
    }

#define CMP(a, b) ((a == b) ? CMP_EQ : ((a > b) ? CMP_GT : CMP_LT))

// Compare two registers
#define CMP_REG_REG(ip, type)                                                  \
    {                                                                          \
        T_u8 r1 = MEM_READ(ip, T_u8);                                          \
        ERR_CHECK_REG(r1) else {                                               \
            ip += sizeof(T_u8);                                                \
            T_u8 r2 = MEM_READ(ip, T_u8);                                      \
            ERR_CHECK_REG(r2) else {                                           \
                ip += sizeof(T_u8);                                            \
                cpu->regs[REG_CMP] =                                           \
                    CMP(*(type *)(cpu->regs + r1), *(type *)(cpu->regs + r2)); \
            }                                                                  \
        }                                                                      \
    }

// Compare a register and a literal
#define CMP_REG_LIT(ip, type)                                          \
    {                                                                  \
        T_u8 reg = MEM_READ(ip, T_u8);                                 \
        ERR_CHECK_REG(reg) else {                                      \
            ip += sizeof(T_u8);                                        \
            type lit = MEM_READ(ip, type);                             \
            ip += sizeof(type);                                        \
            cpu->regs[REG_CMP] = CMP(*(type *)(cpu->regs + reg), lit); \
        }                                                              \
    }

// Compare two memory addresses
#define CMP_MEM_MEM(ip)                                        \
    {                                                          \
        T_u8 bytes = MEM_READ(ip, T_u8);                       \
        ip += sizeof(T_u8);                                    \
        UWORD_T addr1 = MEM_READ(ip, UWORD_T);                 \
        ip += sizeof(UWORD_T);                                 \
        UWORD_T addr2 = MEM_READ(ip, UWORD_T);                 \
        ip += sizeof(UWORD_T);                                 \
        void *buf1 = (void *)((T_u8 *)cpu->mem + addr1);       \
        void *buf2 = (void *)((T_u8 *)cpu->mem + addr2);       \
        cpu->regs[REG_CMP] = bytes_compare(buf1, buf2, bytes); \
    }

// Jump to a literal if `REG_CMP op flag` is true
#define JMP_LIT_IF(ip, op, flag)        \
    {                                   \
        if (cpu->regs[REG_CMP] op flag) \
            SET_LIT(ip, ip, UWORD_T)    \
        else                            \
            ip += sizeof(UWORD_T);      \
    }

// Jump to a register if `REG_CMP op flag` is true
#define JMP_REG_IF(ip, op, flag)        \
    {                                   \
        if (cpu->regs[REG_CMP] op flag) \
            SET_REG(ip, ip, UWORD_T)    \
        else                            \
            ip += sizeof(UWORD_T);      \
    }

/** Begin a fetch-execute cycle, starting at `ip`. Continue until error of HALT.
 * Return number of cycles. */
unsigned int cpu_fecycle(struct CPU *cpu);

#endif