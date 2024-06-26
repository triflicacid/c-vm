#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include "util.h"

// Check if memory address is valid. Expects defined `CPU cpu`
#define MEM_CHECK(addr) (addr >= 0 && addr < cpu->mem_size)

// Macro - easy memory read. Requires variable `CPU cpu` to be defined.
// `addr` is not modified
#define MEM_READ(addr, type) (*(type *)((char *)cpu->mem + addr))

// Macro - easy memory write. Requires variable `CPU cpu`. `addr` is
// incremented by `sizeof(type)`
#define MEM_WRITE(addr, type, value)                  \
    {                                                 \
        (*(type *)((char *)cpu->mem + addr) = value); \
        (addr += sizeof(type));                       \
    }

// Macro - easy memory write. Requires variable `CPU cpu`. `addr` is
// incremented by `sizeof(type * bytes)`. `ptr` points to start of block to move
// (`void*`)
#define MEM_WRITE_BYTES(addr, ptr, bytes) \
    for (T_u8 i = 0; i < bytes; ++i) MEM_WRITE(addr, T_u8, *((T_u8 *)ptr + i))

// Macro - easy memory write. Requires variable `CPU cpu`. `addr` is not
// modified
#define MEM_WRITEK(addr, type, value) \
    (*(type *)((char *)cpu->mem + addr) = value)

typedef struct CPU * CPU;

#include "err.h"
#include "opcodes.h"
#include "registers.h"
#include "binary_header.h"

/** Create a new CPU struct */
CPU cpu_create(WORD_T mem_size);

/** Change CPU stack size */
void cpu_set_stack_size(CPU cpu, WORD_T size);

/** Change CPU output file (redirect stdout) */
void cpu_set_fout(CPU cpu, FILE *out);

/** Destroy data inside a struct. Doesn't free() the struct itself. Doesn't
 * close file pointers. */
void cpu_destroy(CPU cpu);

/** Print CPU details */
void cpu_print_details(CPU cpu);

/** Print into CPUs memory, specifying how many addresses to show per line.
 * `length` specified number of words to print. */
void cpu_mem_print(CPU cpu, WORD_T addr_start, unsigned int length, unsigned char word_size,
        int per_line);

/** Read data from CPUs memory into `data` (array of bytes, `length` is its
 * length). Return error code (or 0). */
ERRNO_T cpu_mem_read(CPU cpu, WORD_T addr_start, void *data, unsigned int length);

/** Read data from file. Return success. */
int cpu_load_file_into_mem(CPU cpu, FILE *fp, WORD_T addr_start, size_t length);

/** Write multiple data to CPUs memory. `data` is array of bytes, `length` of
 * number of bytes. Return error code (or 0). */
ERRNO_T cpu_write_data_into_mem(CPU cpu, WORD_T addr_start, const void *data, unsigned int data_length);

/** Print register contents */
int cpu_reg_print(CPU cpu);

/** Set contents of register */
void cpu_reg_write(CPU cpu, unsigned int reg_offset, WORD_T value);

/** Get contents of register Instruction Pointer */
WORD_T cpu_reg_read(CPU cpu, unsigned int reg_offset);

/** Execute given opcode. If needed, fetched data from cpu.mem, using `ip`
 * as the inst instruction pointer (on invocation, if in contiguous memory, `ip`
 * must point to cell AFTER instruction). `ip` is changed accordingly. Return
 * whether to continue execution. */
int cpu_execute_opcode(CPU cpu, OPCODE_T opcode, WORD_T *ip);

/** Execute the instruction pointed to by CPUs `ip`. Return if continue
 * execution. */
int cpu_execute(CPU cpu);

/** Execute a syscall, return if we should continue execution. */
int cpu_syscall(CPU cpu, int op);

/** Print the contents of a CPUs stack */
void cpu_stack_print(CPU cpu);

/** Print the contents of the topmost CPU stack frame. */
void cpu_stack_frame_print(CPU cpu);

/** Print CPU error information */
void cpu_err_print(CPU cpu);

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
#define MOV_MEM_REG(ip, type)                  \
    {                                          \
        UWORD_T addr = MEM_READ(ip, UWORD_T);  \
        ip += sizeof(UWORD_T);                 \
        T_u8 reg = MEM_READ(ip, T_u8);         \
        ERR_CHECK_REG(reg) else {              \
            ip += sizeof(T_u8);                \
            type data = MEM_READ(addr, type);  \
            cpu->regs[reg] = *(WORD_T *)&data; \
        }                                      \
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

// Move value of type `type` from [register + lit] to another
// register
#define MOV_LIT_OFF_REG(ip, type)                                \
    {                                                            \
        T_u8 r1 = MEM_READ(ip, T_u8);                            \
        ERR_CHECK_REG(r1)                                        \
        else                                                     \
        {                                                        \
            ip += sizeof(T_u8);                                  \
            WORD_T lit = MEM_READ(ip, WORD_T);                   \
            ip += sizeof(WORD_T);                                \
            T_u8 r2 = MEM_READ(ip, T_u8);                        \
            ERR_CHECK_REG(r2)                                    \
            else                                                 \
            {                                                    \
                ip += sizeof(T_u8);                              \
                type data = MEM_READ(lit + cpu->regs[r1], type); \
                cpu->regs[r2] = data;                            \
            }                                                    \
        }                                                        \
    }

// Perform operation between register and literal : reg = reg op lit
#define OP_REG_LIT(op, ip, litT, extra)             \
    {                                               \
        T_u8 reg = MEM_READ(ip, T_u8);              \
        ERR_CHECK_REG(reg) else {                   \
            ip += sizeof(T_u8);                     \
            litT lit = MEM_READ(ip, litT);          \
            ip += sizeof(litT);                     \
            cpu->regs[reg] = cpu->regs[reg] op lit; \
            extra                                   \
        }                                           \
    }

// Perform operation between register and a literal in type `type` : r1 = r1 op1
// lit, r3 = r1 op2 lit
#define OP_REG_LIT_REG(op1, op2, ip, type, r2)             \
    {                                                      \
        T_u8 r1 = MEM_READ(ip, T_u8);                      \
        ERR_CHECK_REG(r1) else {                           \
            ip += sizeof(T_u8);                            \
            type lit = MEM_READ(ip, type);                 \
            ip += sizeof(type);                            \
            type v1 = (*(type *)(cpu->regs + r1)) op1 lit; \
            type v2 = (*(type *)(cpu->regs + r1)) op2 lit; \
            cpu->regs[r1] = *(WORD_T *)&v1;                \
            cpu->regs[r2] = *(WORD_T *)&v2;                \
        }                                                  \
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
#define OP_REG_REG(op, ip, type, extra)                                      \
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
                extra                                                        \
            }                                                                \
        }                                                                    \
    }

// Perform operation on one register : reg = pre reg post
#define OP_REG(pre, post, ip, type, extra)                 \
    {                                                      \
        T_u8 reg = MEM_READ(ip, T_u8);                     \
        ERR_CHECK_REG(reg) else {                          \
            ip += sizeof(T_u8);                            \
            type v = pre * (type *)(cpu->regs + reg) post; \
            cpu->regs[reg] = *(WORD_T *)&v;                \
            extra                                          \
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
#define ARS_REG(ip, extra)                                         \
    {                                                              \
        T_u8 r1 = MEM_READ(ip, T_u8);                              \
        ERR_CHECK_REG(r1) else {                                   \
            ip += sizeof(T_u8);                                    \
            T_u8 r2 = MEM_READ(ip, T_u8);                          \
            ERR_CHECK_REG(r2) else {                               \
                ip += sizeof(T_u8);                                \
                cpu->regs[r1] = ARS(cpu->regs[r1], cpu->regs[r2]); \
                extra                                              \
            }                                                      \
        }                                                          \
    }

// Perform arithmetic right shift : reg = reg a>> lit
#define ARS_LIT(ip, type, extra)                       \
    {                                                  \
        T_u8 reg = MEM_READ(ip, T_u8);                 \
        ERR_CHECK_REG(reg) else {                      \
            ip += sizeof(T_u8);                        \
            type lit = MEM_READ(ip, type);             \
            ip += sizeof(type);                        \
            cpu->regs[reg] = ARS(cpu->regs[reg], lit); \
            extra                                      \
        }                                              \
    }

// Convert register form one data-type to another. dt1 -> dt2
#define OP_CVT(ip, dt1, dt2)                                        \
    {                                                               \
        T_u8 reg = MEM_READ(ip, T_u8);                              \
        ERR_CHECK_REG(reg) else {                                   \
            ip += sizeof(T_u8);                                     \
            dt2 v = (dt2)(*(dt1 *)(&cpu->regs[reg]));               \
            *(dt2 *)((char *)cpu->regs + reg * sizeof(WORD_T)) = v; \
        }                                                           \
    }

// Instruction syntax `<bytes: u8> <addr: uword>`. In-place modify `addr` result
// of `fname(addr, bytes)`
#define OP_APPLYF_MEM(ip, fname, extra)                  \
    {                                                    \
        T_u8 bytes = MEM_READ(ip, T_u8);                 \
        ip += sizeof(T_u8);                              \
        UWORD_T addr = MEM_READ(ip, UWORD_T);            \
        ip += sizeof(UWORD_T);                           \
        fname((void *)((T_u8 *)cpu->mem + addr), bytes); \
        extra                                            \
    }

// Instruction syntax `<bytes: u8> <lit: ...>`. Call `fname`.
#define OP_APPLYF_LIT(ip, fname)                       \
    {                                                  \
        T_u8 bytes = MEM_READ(ip, T_u8);               \
        ip += sizeof(T_u8);                            \
        fname((void *)((T_u8 *)cpu->mem + ip), bytes); \
        ip += bytes;                                   \
    }

// Instruction syntax `<bytes: u8> <addr1: uword> <addr2: uword>`. In-place
// modify `addr1` result of `fname(addr1, addr2, addr1, bytes)`. Set `retVar` to
// return value of `fname`.
#define OP_APPLYF_MEM_MEM(ip, fname, extra)              \
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
        extra                                            \
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

// Compare two literals
#define CMP_LIT_LIT(ip, type)                 \
    {                                         \
        type lit1 = MEM_READ(ip, type);       \
        ip += sizeof(type);                   \
        type lit2 = MEM_READ(ip, type);       \
        ip += sizeof(type);                   \
        cpu->regs[REG_CMP] = CMP(lit1, lit2); \
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

// Push a value onto the stack
#define PUSH(type, value)                                                  \
    {                                                                      \
        *(type *)((T_u8 *)cpu->mem + (cpu->regs[REG_SP] - sizeof(type))) = \
            value;                                                         \
        cpu->regs[REG_SP] -= sizeof(type);                                 \
        ERR_CHECK_STACK_OFLOW();                                           \
    }

// Push a literal onto the stack
#define PUSH_LIT(ip, type)                 \
    {                                      \
        ERR_CHECK_STACK_OFLOW() else {     \
            type lit = MEM_READ(ip, type); \
            ip += sizeof(type);            \
            PUSH(type, lit);               \
        }                                  \
    }

// Push a register onto the stack
#define PUSH_REG(ip, type)                              \
    {                                                   \
        ERR_CHECK_STACK_OFLOW() else {                  \
            T_u8 reg = MEM_READ(ip, T_u8);              \
            ERR_CHECK_REG(reg) else {                   \
                ip += sizeof(T_u8);                     \
                PUSH(type, *(type *)(cpu->regs + reg)); \
            }                                           \
        }                                               \
    }

// Push value at memory address onto the stack
#define PUSH_MEM(ip, type)                         \
    {                                              \
        ERR_CHECK_STACK_OFLOW() else {             \
            UWORD_T addr = MEM_READ(ip, UWORD_T);  \
            ERR_CHECK_ADDR(addr) else {            \
                ip += sizeof(UWORD_T);             \
                type value = MEM_READ(addr, type); \
                PUSH(type, value);                 \
            }                                      \
        }                                          \
    }

// Push n-bytes to the stack: `<bytes: u8> <lit: ...>`
#define PUSHN_LIT(ip)                                                      \
    {                                                                      \
        ERR_CHECK_STACK_OFLOW() else {                                     \
            T_u8 nbytes = MEM_READ(ip, T_u8);                              \
            ip += sizeof(nbytes);                                          \
            for (UWORD_T off = 0; off < nbytes; ++off) {                   \
                if (ip >= cpu->mem_size) {                                 \
                    ERR_SET(ERR_MEMOOB, ip);                               \
                    break;                                                 \
                }                                                          \
                *((T_u8 *)cpu->mem + (cpu->regs[REG_SP] - sizeof(T_u8))) = \
                    *((T_u8 *)cpu->mem + ip);                              \
                cpu->regs[REG_SP] -= sizeof(T_u8);                         \
                ip += sizeof(T_u8);                                        \
            }                                                              \
        }                                                                  \
        ERR_CHECK_STACK_OFLOW();                                           \
    }

// Push n-bytes to the stack: `<bytes: u8> <addr: uword>`
#define PUSHN_MEM(ip)                                                      \
    {                                                                      \
        ERR_CHECK_STACK_OFLOW() else {                                     \
            T_u8 nbytes = MEM_READ(ip, T_u8);                              \
            ip += sizeof(nbytes);                                          \
            UWORD_T addr = MEM_READ(ip, UWORD_T);                          \
            ip += sizeof(UWORD_T);                                         \
            for (UWORD_T off = 0; off < nbytes; ++off) {                   \
                if (addr + off >= cpu->mem_size) {                         \
                    ERR_SET(ERR_MEMOOB, addr + off);                       \
                    break;                                                 \
                }                                                          \
                *((T_u8 *)cpu->mem + (cpu->regs[REG_SP] - sizeof(T_u8))) = \
                    *((T_u8 *)cpu->mem + addr + off);                      \
                cpu->regs[REG_SP] -= sizeof(T_u8);                         \
            }                                                              \
        }                                                                  \
        ERR_CHECK_STACK_OFLOW();                                           \
    }

// Pop value `type` from stack. Set to `var`.
#define POP(type, var)                                     \
    var = *(type *)((T_u8 *)cpu->mem + cpu->regs[REG_SP]); \
    cpu->regs[REG_SP] += sizeof(type);                     \
    ERR_CHECK_STACK_UFLOW();

// Pop `type` off stack into register
#define POP_REG(ip, type)                     \
    {                                         \
        T_u8 reg = MEM_READ(ip, T_u8);        \
        ERR_CHECK_REG(reg) else {             \
            ip += sizeof(T_u8);               \
            type val;                         \
            POP(type, val);                   \
            *(type *)(cpu->regs + reg) = val; \
        }                                     \
    }

// Pop n-bytes from stack and write to memory address
#define POPN_MEM(ip)                                  \
    {                                                 \
        T_u8 nbytes = MEM_READ(ip, T_u8);             \
        ip += sizeof(T_u8);                           \
        UWORD_T addr = MEM_READ(ip, UWORD_T);         \
        ip += sizeof(UWORD_T);                        \
        T_u8 value;                                   \
        for (UWORD_T off = 0; off < nbytes; ++off) {  \
            if (addr + off >= cpu->mem_size) {        \
                ERR_SET(ERR_MEMOOB, addr + off);      \
                break;                                \
            }                                         \
            POP(T_u8, value);                         \
            *((T_u8 *)cpu->mem + addr + off) = value; \
        }                                             \
    }

// Print register as `type` via printf() using the provided formatting flag
#define PRINT_REG(ip, type, flag)                     \
    {                                                 \
        T_u8 reg = MEM_READ(ip, T_u8);                \
        ERR_CHECK_REG(reg) else {                     \
            ip += sizeof(T_u8);                       \
            printf(flag, *(type *)(cpu->regs + reg)); \
        }                                             \
    }

/** Load binary header */
void cpu_load_header(CPU cpu, struct binary_header_data *header);

/** Begin a fetch-execute cycle, starting at `ip`. Continue until error of HALT.
 * Return number of cycles. */
unsigned int cpu_fetch_execute_cycle(CPU cpu);

#endif