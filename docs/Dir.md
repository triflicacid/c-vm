# Directory

This file explains certain files' usage in `src/`.


- `cpu`. Source code for `struct CPU`
  - `bit-ops.h`
  - `bit-ops.c`. Contains various functions for manipulating variable-length byte buffers
  - `cpu.h`
  - `cpu.c`. Functions for creating a CPU, memory and register I/O etc...
  - `err.h`. Defines error-code constants for `cpu.err`
  - `err.c`. Functions for printing errors from `cpu.err`
  - `fetch-exec.h`. Defines MACROs for various opcode-operations
  - `fetch-exec.c`. Functions for fetching and executing opcodes
  - `opcodes.h`. Defines constants for opcodes
- `util.h`. Defines generic useful constants
- `util.c`. Defines generic useful functions