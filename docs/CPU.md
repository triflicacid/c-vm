# CPU

The CPU is the core machinery of the virtual machine. Its role is handle the fetching and execution of instructions.
In this project, we have `struct CPU` with contains a pointer to the VMs memory, its size, as well as its register bank and error status.

A `CPU` struct may be created by calling `cpu_create`. As the CPUs memory is `malloc`d, a CPU must be disposed of via `cpu_destroy`.

## Registers

Registers are a small single-word storage slot.

See `Register.md`.

## Memory

The amount (in bytes) of memory is passed as an argument to `cpu_create` and is stored as `cpu.mem_size`.
A pointer to the beginning of the memory block is `cpu.mem`.

A `word` is the maximum amount of bytes that may be transferred by one instruction (excluding memory-based n-byte operations), and is `WORD_T`. Its unsigned counterpart is `UWORD_T`. Most instructions come in a word variant, and explicit variants. For example, `OP_MOV_...` for moving words, and `OP_MOVn_...` for moving `n` bytes (one of `8`, `16`, `32` or `64`).

## Building
To build the processor, run CMake using `processor/CMakeLists.txt`.
By default, the output is `processor/bin/`.

## Execution

The file `execute.c` reads a binary source file and executes it on the CPU.

To execute, run `./<bin> [<file>] [-p] [-m <size>] [-s <size>]`
  - `file` is the source file. Default is `source.bin`.
  - `-d` enables the printing of extra information.
  - `-m` sets the CPUs memory size to `size`.
  - `-o` sets output file (STDOUT). Note, this is only for output caused by instructions, and not error/debug info.
  - `-s` sets the CPUs stack size to `size`.

## Binary Layout

Compiled binary files have the following structure:
- `start_offset: WORD` - address offset of program start.
- `program: BYTES` - program instructions/data.

## Error Codes
These are errors which are thrown during machine code execution. The error code is stored in `REG_ERR` and associated error data is stored in `REG_FLAG`.

| error             | REG_FLAG        | Description                                      |
|-------------------|-----------------|--------------------------------------------------|
| `ERR_NONE`        | *N/A*           | No error                                         |
| `ERR_MEMOOB`      | Memory address  | Attempted to access out-of-bounds memory address |
| `ERR_REG`         | Register offset | Attempted to access an illegal register          |
| `ERR_UNINST`      | Opcode          | Encountered illegal opcode during FE-cycle       |
| `ERR_STACK_UFLOW` | *N/A*           | Attempted to POP of an empty stack               |
| `ERR_STACK_OFLOW` | Memory address  | Stack has overflown - size exceeds capacity      |
