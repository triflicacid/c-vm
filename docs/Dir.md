# Directory

This file explains certain files' usage in `src/`.


- `assembler`. Source code for assembler
  - `args.h`. Definitions for Assembler Arguments
  - `args.c`. Defines functions for handling `struct AsmArgument`. Represent arguments to assembler mnemonics
  - `assemble.h`
  - `assemble.c`. Main file for assembling a source file into a machine code buffer
  - `chunk.h`
  - `chunk.c`. Functions for manipulating "chunks" of data - used in AST
  - `err.h`. Defines error constants which may be returned during a failed assembly
  - `instruction.h`. Definitions for Assembler Instructions
  - `instruction.c`. Defines functions for handling `struct AsmInstruction`. Represents assembly mnemonics
  - `labels.h`
  - `labels.c`. Functions for handling `struct AsmLabel` in assembly. Represents an assembly label
  - `line.h`
  - `line.c`. Functions for handling `struct AsmLine` in assembly. Represents a source line in an assembly source file
  - `symbol.h`
  - `symbol.c`. Functions for handling `struct AsmSymbol` in assembly. Represents constants defined whilst assembling.
- `cpu`. Source code for the CPU
  - `bit-ops.h`
  - `bit-ops.c`. Contains various functions for manipulating variable-length byte buffers
  - `cpu.h`
  - `cpu.c`. Functions for creating a CPU, memory and register I/O etc...
  - `err.h`. Defines error-code constants for `cpu.err`
  - `err.c`. Functions for printing errors from `cpu.err`
  - `fetch-exec.h`. Defines MACROs for various opcode-operations
  - `fetch-exec.c`. Functions for fetching and executing opcodes
  - `opcodes.h`. Defines constants for opcodes
- `linked-list.h` - Provide macros for creating functions involving the creation, destruction and traversal of linked lists
- `util.h`. Defines generic useful constants
- `util.c`. Defines generic useful functions