#ifndef __CPU_H__
#define __CPU_H__

#include <stdio.h>

#include "../util.h"
#include "opcodes.h"

#define WORD_T T_i64
#define WORD_T_FLAG "%lli"
#define UWORD_T T_u64
#define UWORD_T_FLAG "%llu"
#define ERRNO_T int

// Registers: general 0-9, ip
// Register for flags
#define REG_FLAG 9
// Register for comparison operations
#define REG_CMP 9
// Instruction pointer
#define REG_IP 10
#define REG_COUNT 11

// Check if memory address is valid. Expects defined `struct CPU *cpu`
#define MEM_CHECK(addr) (addr >= 0 && addr < cpu->mem_size)

// Macro - easy memory read. Requires variable `struct CPU *cpu` to be defined.
// `addr` is not modified
#define MEM_READ(addr, type) (*(type *)((char *)cpu->mem + addr))

// Macro - easy memory write. Requires variable `struct CPU *cpu`. `addr` is
// incremented by `sizeof(type)`
#define MEM_WRITE(addr, type, value)                  \
    {                                                 \
        (*(type *)((char *)cpu->mem + addr) = value); \
        (addr += sizeof(type));                       \
    }

// Macro - easy memory write. Requires variable `struct CPU *cpu`. `addr` is
// incremented by `sizeof(type * bytes)`. `ptr` points to start of block to move
// (`void*`)
#define MEM_WRITE_BYTES(addr, ptr, bytes) \
    for (T_u8 i = 0; i < bytes; ++i) MEM_WRITE(addr, T_u8, *((T_u8 *)ptr + i))

// Macro - easy memory write. Requires variable `struct CPU *cpu`. `addr` is not
// modified
#define MEM_WRITEK(addr, type, value) \
    (*(type *)((char *)cpu->mem + addr) = value)

struct CPU {
    WORD_T mem_size;         // Size of .mem
    void *mem;               // Pointer to start of memory block
    ERRNO_T err;             // Error number (0 = none)
    WORD_T err_data;         // Extra error data if error (errno != 0)
    WORD_T regs[REG_COUNT];  // Register memory
};

/** Create a new CPU struct */
struct CPU cpu_create(UWORD_T mem_size);

/** Destroy data inside a struct. Doesn't free() the struct itself */
void cpu_destroy(struct CPU *cpu);

/** Print CPU details */
void cpu_print_details(struct CPU *cpu);

/** Print into CPUs memory, specifying how many addresses to show per line.
 * `length` specified number of words to print. */
void cpu_mem_print(const struct CPU *cpu, const UWORD_T addr_start,
                   const unsigned int length, const unsigned char word_size,
                   const int per_line);

/** Read data from CPUs memory into `data` (array of bytes, `length` is its
 * length). Return error code (or 0). */
ERRNO_T cpu_mem_read(struct CPU *cpu, UWORD_T addr_start, void *data,
                     unsigned int length);

/** Read data from file. Return success. */
int cpu_mem_fread(struct CPU *cpu, FILE *fp, UWORD_T addr_start, size_t length);

/** Write multiple data to CPUs memory. `data` is array of bytes, `length` of
 * number of bytes. Return error code (or 0). */
ERRNO_T cpu_mem_write_array(struct CPU *cpu, UWORD_T addr_start,
                            const void *data, unsigned int length);

/** Print register contents */
ERRNO_T cpu_reg_print(struct CPU *cpu);

/** Set contents of register */
void cpu_reg_write(struct CPU *cpu, unsigned int reg_offset, WORD_T value);

/** Get contents of register Instruction Pointer */
WORD_T cpu_reg_read(struct CPU *cpu, unsigned int reg_offset);

/** Execute given opcode. If needed, fetched data from cpu.mem, using `ip`
 * as the inst instruction pointer (on invocation, if in contiguous memory, `ip`
 * must point to cell AFTER instruction). `ip` is changed accordingly. Return
 * whether to continue execution. */
int cpu_mem_exec(struct CPU *cpu, OPCODE_T opcode, UWORD_T *ip);

/** Execute the instruction pointed to by CPUs `ip`. Return if continue
 * execution. */
int cpu_exec(struct CPU *cpu);

#endif