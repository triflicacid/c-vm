# Assembler

The assembler takes in a file of assembly code and outputs its equivalent machine code. The assembler is still in development.

## Execution

**This is open to change**

To execute the assembler, first compile it by running `compile.bat`.

Then, execute `./main.exe [file]` where `file` is the path to the assembly source file to assemble. If none is provided, `file="source.asm"`.

## Syntax

Assembly source files are read line-by-line.

Instructions come in the form `mnemonic [...args]` where the arguments consist of a space- or comma-seperated list of:
  - `nnn`, where `nnn` is a number, represents a **literal**
  - `[nnn]`, where `nnn` is a number, represents an **address**
  - `abc`, where `abc` is a string of characters, represents a **register**. The string is translated to its corresponsing register offset or, if it is not recognised, will result in an error.
  - `[abc]`, where `abc` is a string of characters, represents a **register pointer**. The string is translated to its corresponsing register offset or, if it is not recognised, will result in an error.

See `Instructions.md` for a list of all implemeted instructions.

### Numeric Values

Numeric values consist of a sequence of digits optionally suffixed by a radix indicator, where
  - `b` or `y`: binary (2)
  - `o` or `q`: octal (8)
  - `d` or `t` or *none*: decimal (10)
  - `h`: hexadecimal (16)

The validity of the digits is dependent upon the radix chosen e.g. `Fh` is valid, but `F` is not.

### Comments

**THIS IS NOT YET IMPLEMENTED**

Comments start with a `;`, with everything after the semi-colon being ignored up until the next lien