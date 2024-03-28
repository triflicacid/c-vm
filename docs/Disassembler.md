# Disassembler

The disassembler takes in a binary file and outputs an assembly source with the same meaning.
THe disassembler is located in `assembler/`.

## Building

To build the assembler, run CMake using `assembler/CMakeLists.txt`.
By default, the output is `assembler/bin/`.

## Execution

To execute, run `./disassembler.exe <src> [options]` where
  - `src` is the path to the assembly source file to assemble.
  - `-d` switches on debug, where assembly progress will be logged.
  - `-o <file>` specifies an output file for machine code. If none is provided, defaults to `source.bin`.
  - `--format-data` formats data (`u8 ...`) nicely (e.g., strings, hex).
  - `--no-labels` disabled insertion of labels (except `main`).
  - `--commas` inserts commas between arguments and data constants.

## Disassembling

Bytes are read as a potential opcode. If the opcode exists, the instruction is extracted and destructured.
Otherwise, the opcode is taken to be raw data and added to a `u8 ...` clause.

### Labels

- The label `main` will be placed at the program's start address, unless the start address is `+0`.
- If a literal or address argument points to an address at which a data segment resides, a label will be inserted e.g., `data0`.
- For a literal/address which appears in a jump instruction, a label will be placed at this address e.g., `pos0`.

Label insertion may be disabled via `--no-labels` (the `main` label is an exception).
