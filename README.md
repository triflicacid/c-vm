# C Virtual Machine

Basic virtual machine written in C.

For more informations, see `docs/`, `tests/`.

## Components
- Basic "CPU"
- Basic assembler (*in progress*)
  - Source is `assemble.c`. See `docs/Assembler.md` for more.

### TODO
- CPU: implement instructions OP_PUSH_REGPTR, OP_POP_REGPTR
- Assembler
  - Defining data. Reserves space in Chunk linked list.
    - Support for floating-point `f32` and `f64`
    - Support to find length of previously defined constant
  - Resolve `removed const` qualifier warnings. `parse_data.h`.
- Print to STDOUT (using memory mapping to map characters?)
- Basic de-compiler? Given .bin, produce .asm

### Bugs

## Goal
The goal of this repo is to build a basic virtual machine with I/O, equipped with a custom assembler, and a C-like language to accompany it.