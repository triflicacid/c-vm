# Assembler

The assembler takes in a file of assembly code and outputs its equivalent machine code.
THe assembler is located in `assembler/`.

## Building

To build the assembler, run CMake using `assembler/CMakeLists.txt`.
By default, the output is `assembler/bin/`.

## Execution

To execute, run `./<bin> [src] [options]` where
  - `src` is the path to the assembly source file to assemble.
  - `-d` switches on detail. Detail about the source file, line/col reached, error messages etc... are printed. If disabled, only the error number is printed.
  - `-d` enables debug mode, if it appears for a second time. Debug information around each assembler stage etc... is printed.
  - `-o <file>` specifies an output file for machine code. If none is provided, defaults to `source.bin`.
  - `-p <file>` specifies an output file for post-processed assembly. This will output the assembly after the pre-processor has dealt with the source. If the flag is stated, but no input file is provided, `preproc.asm` is used.

## Assembling

The assembler works in the current way:

- Read file and split into lines
- Scan each line for pre-processor directives
  - If `-p` switch enabled, this would be outputted 
- Parse each line into an AST
  - Split into "chunks"
  - Resolve labels
- Resolve labels in AST
- Convert into machine code and insert into a buffer

## Pre-Processor

These are evaluated before any assembling takes place

- `%define [SYMBOL] [VALUE]` - defines the constant `SYMBOL` with value `VALUE`. `VALUE` contains everything from after `SYMBOL ` to end of the line. From this point, any occurances of `SYMBOL` is replaced by `VALUE`.
- `%ignore` - ignore this line. Like a comment.
- `%stop` - ignore anything after this point. "Stops" the assembler.

## Syntax

Assembly source files are read line-by-line, and have the following syntax: `[label:] [mnemonic [...args]] [; Comment]`. Trailing whitespace is removed, and blank/empty lines are ignored.

Labels come in the form `label:`
  - `label` is a word containing letters and numbers. (See section on labels for more.)

If `mnemonic` is one of `u8, u16, u32, u64`, this is a data constant.
  - `args` is a comma- or space-seperated list of constants. If no `args` are provided, insert a `0`.
    - `...[r]` - Numeric constants. These may be followed by a radix suffix `r`.  
    - `'...'` - Character constants
    - `"..."` - String constants
  - The mnemonic specifies the type of each item. Each item is written to the output with this type. In the case of strings, each character in said string has this type.

Instructions come in the form `mnemonic [...args]` where the arguments consist of a comma-seperated list of:
  - `nnn`, where `nnn` is a number, represents a **literal**
  - `[nnn]`, where `nnn` is a number, represents an **address**
  - `abc`, where `abc` is a string of characters, represents a **register**. The string is translated to its corresponsing register offset or, if it is not recognised, will result in an error.
  - `[abc]`, where `abc` is a string of characters, represents a **register pointer**. The string is translated to its corresponsing register offset or, if it is not recognised, will result in an error.
  - `'c'`, where `c` is a character (or escape sequence), represents a **literal**. If multiple character literals follow eachother, they will be concatenated to an integer. Maximum is 8 characters.
  - `"..."`, where `...` is a string, represents a **literal**. Maximum length is 8 characters.

See `Instructions.md` for a list of all implemeted instructions.

Comments start with a `;`, with everything after the semi-colon being ignored up until the next line

### Escape Literals

- `\b` - Non-destructive backspace (8h)
- `\d...` - Character with code given in decimal
- `\n` - Newline (Ah)
- `\o...` - Character with code given in octal
- `\r` - Carriage Return (Dh)
- `\s` - Space (20h)
- `\t` - Horizontal Tab (9h)
- `\v` - Vertical Tab (Bh)
- `\x...` - Character with code given in hexadecimal
- `\0` - Null (0h)

### Numeric Values

**All numeric constants are 64-bit**

Numeric values consist of a sequence of digits optionally suffixed by a radix indicator, where
  - `b` or `y`: binary (2)
  - `o` or `q`: octal (8)
  - `d` or `t` or *none*: decimal (10)
  - `h`: hexadecimal (16)

The validity of the digits is dependent upon the radix chosen e.g. `Fh` is valid, but `F` is not.

Digits may be seperated by underscores `_`.

Numbers containing `.` will be treated as floating-point.

## Labels

Labels are placeholder names for addresses, and can be substituted in an instruction where an address would be expected.

When defined, it is initialised with a value of the current offset is appeared at. Labels may be re-defined, in which case its offset will be updated accordingly.

When a label is encountered in an instruction...
  - If the label is defined, it is immediately replaced by its address. Replace all un-replaced references to this label.
  - If the label is not defined, this is cached.

If a label is not defined, an error will be reported.

See `assembler/test/labels.asm` for an example.
