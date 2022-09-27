# Instructions

Below is the full list of instructions supported by the assembler.

(See `cpu/opcodes.h` for the list of opcode numerical constants)

| Mnemonic | Fully Qualified Name | Arguments | Description | Example |
| - | - | - | - | - |
| mov | OP_MOV_LIT_REG | `<lit: word>`, `<reg: u8>` | Move literal word into register `reg` | `mov 100h, r3` |
| mov | OP_MOV_LIT_MEM | `<lit: word>`, `<addr: uword>` | Move literal to address | `mov 100h, [128]` |
| mov | OP_MOV_MEM_REG | `<addr: uword>`, `<reg: u8>` | Move value at address to register | `mov [1Fh], r2` |
| mov | OP_MOV_REG_MEM | `<reg: u8>`, `<addr: uword>` | Move value in register to address | `mov r2, [1Fh]` |
| mov | OP_MOV_REGPTR_REG | `<regptr: u8>`, `<reg: u8>` | Move value at memory address stored in first register to second register | `mov [r1], r2` |
| mov | OP_MOV_REG_REGPTR | `<reg: u8>`, `<regptr: u8>` | Move value in first register to memory address stored in the second register | `mov r1, [r2]` |
| mov | OP_MOV_REG_REG | `<reg: u8>`, `<reg: u8>` | Move value in first register to second register | `mov r1, r2` |
| hlt | OP_HALT | | Stop execution | `hlt` |