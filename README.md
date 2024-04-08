# C Virtual Machine

Basic virtual machine written in C.

For more information, see `docs/`, `tests/`.

## Components
- Basic "CPU"
- Assembler
    - Source is `assembler/assembler.cpp`. See `docs/Assembler.md` for more.
- Disassembler
  - Source is `assembler/disassembler.cpp`. See `docs/Disassembler.md` for more.

### TODO
- Assembler
    - Support for expressions - RPN. Include dollar `$` variable for last address.
- Print to STDOUT (using memory mapping to map characters?)
- Compiler to C-like language

### Bugs

## Goal
The goal of this repo is to build a basic virtual machine with I/O, equipped with a custom assembler, and a C-like language to accompany it.