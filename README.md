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
    - Syntax: `[<label>:] <type> <data>`
    - `data` is a comma-seperated list of items. Items may be numbers, characters, or a string. Each item will be inserted as type `type` (includes characters in a string)
    - e.g. `num: f64 3.14`, `string: u8 "Hello"`
- Print to STDOUT (using memory mapping to map characters?)
- Basic de-compiler? Given .bin, produce .asm

### Bugs

## Goal
The goal of this repo is to build a basic virtual machine with I/O, equipped with a custom assembler, and a C-like language to accompany it.