# Instructions

Below is the full list of instructions supported by the assembler.

(See `cpu/opcodes.h` for the list of opcode numerical constants)

| Mnemonic | Fully Qualified Name | Arguments | Description | Example |
| - | - | - | - | - |
| and | OP_AND_REG_LIT | `<reg: u8>`, `<lit: word>` | Compute bitwise AND of register and literal and place the result in register | `and r1, 101b` |
| and | OP_AND_REG_REG | `<reg: u8>`, `<reg: u8>` | Compute bitwise AND of two registers and place in the first register | `and r1, r2` |
| and | OP_AND_MEM_MEM | `<bytes: u8>`, `<addr: uword>`, `<addr: uword>` | Compute bitwise AND of two `byte`-length buffers at the addresses and store result in the first address | `and 12, [200], [212]` |
| hlt | OP_HALT | | Stop execution | `hlt` |
| mov | OP_MOV_LIT_REG | `<lit: word>`, `<reg: u8>` | Move literal word into register `reg` | `mov 100h, r3` |
| mov | OP_MOV_LIT_MEM | `<lit: word>`, `<addr: uword>` | Move literal to address | `mov 100h, [128]` |
| mov | OP_MOV_MEM_REG | `<addr: uword>`, `<reg: u8>` | Move value at address to register | `mov [1Fh], r2` |
| mov | OP_MOV_REG_MEM | `<reg: u8>`, `<addr: uword>` | Move value in register to address | `mov r2, [1Fh]` |
| mov | OP_MOV_REGPTR_REG | `<regptr: u8>`, `<reg: u8>` | Move value at memory address stored in first register to second register | `mov [r1], r2` |
| mov | OP_MOV_REG_REGPTR | `<reg: u8>`, `<regptr: u8>` | Move value in first register to memory address stored in the second register | `mov r1, [r2]` |
| mov | OP_MOV_REG_REG | `<reg: u8>`, `<reg: u8>` | Move value in first register to second register | `mov r1, r2` |
| neg | OP_NEG | `<reg: u8>` | Negate value in register (twos complement) | `neg r3` |
| negf32 | OP_NEGF32 | `<reg: u8>` | Negate 32-bit floating point value in register | `negf32 r3` |
| negf64 | OP_NEGF64 | `<reg: u8>` | Negate 64-bit floating point value in register | `negf64 r3` |
| not | OP_NOT_REG | `<reg: u8>` | Compute bitwise NOT of a register in-place | `not r3` |
| not | OP_NOT_MEM | `<bytes: u8>`, `<addr: uword>` | Compute bitwise NOT of a `byte`-length buffer at given address in-place | `not 128, [100]` |
| or | OP_OR_REG_LIT | `<reg: u8>`, `<lit: word>` | Compute bitwise OR of register and literal and place the result in register | `or r1, 101b` |
| or | OP_OR_REG_REG | `<reg: u8>`, `<reg: u8>` | Compute bitwise OR of two registers and place sthe result in the first register | `or r1, r2` |
| or | OP_OR_MEM_MEM | `<bytes: u8>`, `<addr: uword>`, `<addr: uword>` | Compute bitwise OR of two `byte`-length buffers at the addresses and store result in the first address | `or 12, [200], [212]` |
| sar | OP_ARSHIFT_LIT | `<reg: u8>`, `<lit: u8>` | Arithmetically shift value in register right `lit` bits | `sar r2, 3` |
| sar | OP_ARSHIFT_REG | `<reg: u8>`, `<reg: u8>` | Arithmetically shift value in register right n-bits, where `n` is value in the second register | `sar r2, r3` |
| sll | OP_LLSHIFT_LIT | `<reg: u8>`, `<lit: u8>` | Logically shift value in register left `lit` bits | `sll r2, 3` |
| sll | OP_LLSHIFT_REG | `<reg: u8>`, `<reg: u8>` | Logically shift value in register left n-bits, where `n` is value in the second register | `sll r2, r3` |
| slr | OP_LRSHIFT_LIT | `<reg: u8>`, `<lit: u8>` | Logically shift value in register right `lit` bits | `slr r2, 3` |
| slr | OP_LRSHIFT_REG | `<reg: u8>`, `<reg: u8>` | Logically shift value in register right n-bits, where `n` is value in the second register | `slr r2, r3` |
| xor | OP_XOR_REG_LIT | `<reg: u8>`, `<lit: word>` | Compute bitwise XOR of register and literal and place the result in register | `xor r1, 101b` |
| xor | OP_XOR_REG_REG | `<reg: u8>`, `<reg: u8>` | Compute bitwise XOR of two registers and place sthe result in the first register | `xor r1, r2` |
| xor | OP_XOR_MEM_MEM | `<bytes: u8>`, `<addr: uword>`, `<addr: uword>` | Compute bitwise XOR of two `byte`-length buffers at the addresses and store result in the first address | `xor 12, [200], [212]` |