# C Virtual Machine

Basic virtual machine written in C.

For more information, see `docs/`, `tests/`.

## Components
- Basic "CPU"
- Basic assembler (*in progress*)
    - Source is `assemble.c`. See `docs/Assembler.md` for more.

### TODO
- CPU
    - Add Breakpoint instruction
    - Implement instructions OP_PUSH_REGPTR, OP_POP_REGPTR
- Assembler
    - Support for expressions - RPN. Include dollar `$` variable for last address.
- Print to STDOUT (using memory mapping to map characters?)
- Basic de-compiler? Given .bin, produce .asm
- Compiler to C-like language

### Bugs

## Goal
The goal of this repo is to build a basic virtual machine with I/O, equipped with a custom assembler, and a C-like language to accompany it.