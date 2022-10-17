# C Virtual Machine

Basic virtual machine written in C.

For more informations, see `docs/`, `tests/`.

## Components
- Basic "CPU"
- Basic assembler (*in progress*)
  - Source is `assemble.c`. See `docs/Assembler.md` for more.

### TODO
- CPU: implement instructions OP_PUSH_REGPTR, OP_POP_REGPTR
- Assembler:
  - floating-point literals
- Print to STDOUT (using memory mapping to map characters?)
- Opcode documentation? (explained in `opcodes.h` - maybe write a more general overview?)
- Basic de-compiler? Given .bin, produce .asm

### Bugs
- `\n` in asm source files

## Goal
The goal of this repo is to build a basic virtual machine with I/O, equipped with a custom assembler, and a C-like language to accompany it.