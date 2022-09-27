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
| or | OP_OR_REG_LIT | `<reg: u8>`, `<lit: word>` | Compute bitwise OR of register and literal and place the result in register | `or r1, 101b` |
| or | OP_OR_REG_REG | `<reg: u8>`, `<reg: u8>` | Compute bitwise OR of two registers and place sthe result in the first register | `or r1, r2` |
| or | OP_OR_MEM_MEM | `<bytes: u8>`, `<addr: uword>`, `<addr: uword>` | Compute bitwise OR of two `byte`-length buffers at the addresses and store result in the first address | `or 12, [200], [212]` |

| xor | OP_XOR_REG_LIT | `<reg: u8>`, `<lit: word>` | Compute bitwise XOR of register and literal and place the result in register | `xor r1, 101b` |
| xor | OP_XOR_REG_REG | `<reg: u8>`, `<reg: u8>` | Compute bitwise XOR of two registers and place sthe result in the first register | `xor r1, r2` |
| xor | OP_XOR_MEM_MEM | `<bytes: u8>`, `<addr: uword>`, `<addr: uword>` | Compute bitwise XOR of two `byte`-length buffers at the addresses and store result in the first address | `xor 12, [200], [212]` |