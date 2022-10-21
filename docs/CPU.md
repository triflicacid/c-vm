# CPU

The CPU is the core machinary of the virtual machine. It's role is handle the fetching and execution of instructions. In this project, we have `struct CPU` with contains a pointer to the VMs memory, its size, as well as its register bank and error status.

A `CPU` struct may be created by calling `cpu_create`. As the CPUs memory is `malloc`d, a CPU must be disposed of via `cpu_destroy`.

## Registers

Registers are a small single-word storage slot. There are `REG_COUNT` registers, all of which are general purpose, except
- `REG_IP` : index of the instruction pointer register. This points to the next instruction to be fetched or, in case of an error, where the data was fetched from.
- `REG_FLAG` : a special register used to output additional data from instructions. Examples include integer under-/over-flow from `n`-byte arithmatic, joind div-mod opertions etc...

## Memory

The amount (in bytes) of memory is passed as an argument to `cpu_create` and is stored as `cpu.mem_size`.
A pointer to the beginning of the memory block is `cpu.mem`.

A `word` is the maximum amount of bytes that may be transferred by one instructions (excluding memory-based n-byte operations), and is `WORD_T`. Its unsigned counterpart is `UWORD_T`. Most instructions come in a word varient, and explicit variants. For example, `OP_MOV_...` for moving words, and `OP_MOVn_...` for moving `n` bytes (one of `8`, `16`, `32` or `64`).

## Execution

The file `execute.c` reads a binary source file and executes it on the CPU.

To compile, run `gcc execute.c -o execute.exe`.

To execute, run `./execute.exe [<file>] [-p] [-m <size>] [-s <size>]`
  - `file` is the source file. Default is `source.bin`.
  - `-d` enables the printing of extra information.
  - `-m` sets the CPUs memory size to `size`.
  - `-o` sets output file (STDOUT). Note, this is only for output caused by instructions, and not error/debug info.
  - `-s` sets the CPUs stack size to `size`.