# Assembler

The assembler takes in a file of assembly code and outputs its equivalent machine code. The assembler is still in development.

## Execution

The source file is `assemble.c`.

To compile, run `gcc assemble.c -o assemble.exe`.

To execute, run `./assemble.exe [src] [options]` where
  - `src` is the path to the assembly source file to assemble. If none is provided, it is defaulted to `source.asm`.
  - `-b <size>` sets the buffer size to `size` bytes. This is the maximum machine code file size. Default is 1MB.
  - `-o <file>` specifies an output file. If none is provided, is id defaulted to `source.bin`.
  - `-p` switches on detail. Detail about the source file, line/col reached, error messages etc... are printed. If disabled, only the error number is printed.

## Syntax

Assembly source files are read line-by-line, and have the following syntax: `[mnemonic [...args]] [; Comment]`. Trailing whitespace is removed, and blank/empty lines are ignored.

Instructions come in the form `mnemonic [...args]` where the arguments consist of a space- or comma-seperated list of:
  - `nnn`, where `nnn` is a number, represents a **literal**
  - `[nnn]`, where `nnn` is a number, represents an **address**
  - `abc`, where `abc` is a string of characters, represents a **register**. The string is translated to its corresponsing register offset or, if it is not recognised, will result in an error.
  - `[abc]`, where `abc` is a string of characters, represents a **register pointer**. The string is translated to its corresponsing register offset or, if it is not recognised, will result in an error.
  - `'c'`, where `c` is a character (or escape sequence), represents a **literal**. If multiple character literals follow eachother, they will be concatenated to an integer.

See `Instructions.md` for a list of all implemeted instructions.

Comments start with a `;`, with everything after the semi-colon being ignored up until the next line

### Numeric Values

Numeric values consist of a sequence of digits optionally suffixed by a radix indicator, where
  - `b` or `y`: binary (2)
  - `o` or `q`: octal (8)
  - `d` or `t` or *none*: decimal (10)
  - `h`: hexadecimal (16)

The validity of the digits is dependent upon the radix chosen e.g. `Fh` is valid, but `F` is not.

