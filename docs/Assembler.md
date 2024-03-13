# Assembler

The assembler takes in a file of assembly code and outputs its equivalent machine code.
THe assembler is located in `assembler/`.

## Building

To build the assembler, run CMake using `assembler/CMakeLists.txt`.
By default, the output is `assembler/bin/`.

## Execution

To execute, run `./assembler.exe <src> [options]` where
  - `src` is the path to the assembly source file to assemble.
  - `-d` switches on debug, where assembly progress will be logged.
  - `-o <file>` specifies an output file for machine code. If none is provided, defaults to `source.bin`.
  - `-p <file>` specifies an output file for post-processed assembly. This will output the assembly after the pre-processor has dealt with the source. If the flag is stated, but no input file is provided, `preproc.asm` is used.
  - `--no-pre-process` skips the pre-processing step.
  - `--no-compile` skips compilation - the file will still be parsed.s

Errors will be printed alongside their status code. See below for a list of possible codes and their meanings.
These may be used in conjunction with error messages to glean insight into the error's nature.

| Code | Name               | Description                                                       |
|------|--------------------|-------------------------------------------------------------------|
| 0    | `None`             | Success; no error.                                                |
| 1    | `Syntax`           | Generic syntax error. See detailed output.                        | 
| 2    | `UnknownMnemonic`  | Unknown mnemonic.                                                 |
| 3    | `BadArguments`     | Unknown argument combination for the mnemonic.                    |
| 4    | `UnknownDirective` | Unknown directive.                                                |
| 5    | `UnknownLabel`     | Reference to unknown label (label not defined).                   |
| 6    | `InvalidLabel`     | Invalid label (invalid name or shadows main label/register name). |
| 7    | `FileNotFound`     | Provided file could not be found/opened with the given operation. |
| 8    | `CircularInclude`  | Circular `%include` detected.                                     |

Note: *internal* errors should not occur and are used for debug purposes only.

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

## Syntax

Assembly source files are read line-by-line, and have the following general syntax:
```
[label:] [mnemonic [...args]] [; Comment]
```

Trailing whitespace is removed, and blank/empty lines are ignored. Lines may also begin with directives which are handles by the pre-processor.

Labels come in the form `label:`
  - `label` is a word containing letters and numbers. (See section on labels for more.)

Code execution starts at the special label `main`.

If `mnemonic` is one of `u8, u16, u32, u64, f32, f64`, this is a data constant.
  - `args` is a comma- or space-seperated list of constants. If no `args` are provided, insert a `0`.
    - `...[r]` - Numeric constants. These may be followed by a radix suffix `r`.  
    - `'...'` - Character constants
    - `"..."` - String constants
  - The mnemonic specifies the type of each item. Each item is written to the output with this type. In the case of strings, each character in said string has this type.

Instructions come in the form `mnemonic [...args]` where the arguments consist of a comma-seperated list of:
  - `nnn`, where `nnn` is a number, represents a **literal**
    - Data type defaults to `i64`. If a decimal point is present, the data type defaults to `f64`.
  - `[nnn]`, where `nnn` is a number, represents an **address**
  - `abc`, where `abc` is a string of characters, represents a **register**. The string is translated to its corresponding register offset or, if it is not recognised, will result in an error.
  - `[abc]`, where `abc` is a string of characters, represents a **register pointer**. The string is translated to its corresponding register offset or, if it is not recognised, will result in an error.
  - `'c'`, where `c` is a character (or escape sequence), represents a **literal**. If multiple character literals follow each other, they will be concatenated to an integer. Maximum is 8 characters.
  - `"..."`, where `...` is a string, represents a **literal**. Maximum length is 8 characters.

See `Instructions.md` for a list of all implemented instructions.

Comments start with a `;`, with everything after the semicolon being ignored up until the next line

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

### Directives
These are instructions to the pre-processor, and are handled before compilation. These modify the source code itself, so may be used for meta-programming.

- `%define [SYMBOL] [VALUE]` - defines the constant `SYMBOL` with value `VALUE`. `VALUE` contains everything from after `SYMBOL ` to end of the line. From this point, any occurrences of `SYMBOL` is replaced by `VALUE`.
```
%define NUMBER 123
mov NUMBER, r0
-->
mov 123, r0
``` 
- `%macro [NAME] <params, ...>` - defines a macro, which is an expandable block of instructions, with the following name. You may provide a list of arguments. All source lines after `%macro` are considered part of the macro's body and **cannot** be more directives -- only `%end` is permitted, which will terminate the macro body.
After definition, when `NAME` is encountered in the `mnemonic` position, supplied arguments are passed to the parameters `<params, ...>`. Any instances of a parameter is replaced by its respective argument in the macro's body. The original line is removed and the modified macro's body is "pasted" in.
```
%macro print_int reg
    mov reg, r1
    mov 0, r0
    syscall
%end
mov 42, r0
print_int r0
-->
mov 42, r0
mov r0, r1
mov 0, r0
syscall
```
- `%rm` - remove this line. Like a comment.
```
%rm hlt
mov 1, r1
-->
mov 1, r1
```      
- `%stop` - ignore anything after this point. "Stops" the assembler.
```
mov 1, r1
%stop
illegal
-->
mov 1, r1
```

## Labels

Labels are placeholder names for addresses, and can be substituted in an instruction where a literal or address would be expected. For example, if label `label=13`:

```
mov label, r0 --> mov 13, r0
mov [label], r0 --> mov [13], r0
```

When defined, it is initialised with a value of the current offset is appeared at. Labels may be re-defined, in which case its offset will be updated accordingly.

When a label is encountered in an instruction...
  - If the label is defined, it is immediately replaced by its value as a literal or an address (if surrounded in `[]`). Replace all un-replaced references to this label.
  - If the label is not defined, this is cached.

If a label is not defined, an error will be reported.

**Restrictions**: a label cannot be the name of a register.

See `assembler/test/labels.asm` for an example.
