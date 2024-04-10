# Language
A lightweight language for the virtual CPU.

This is still under construction.

## Compiling
To compile a source file into assembly, execute:
```
.\bin\language.exe <source> -o <output> [options]
```

All `[options]` are optional, and are described as follows:
- `-d` - Enable debug mode.
- `-l <file>` - Write lexed program to `file` as XML.
- `-p <file>` - Write parsed program to `file` as XML.

## Language Configuration
These options are listed internally, but currently cannot be changed without editing `src/LanguageOptions.hpp`.

### `allow_shadowing`

**Default: `true`**

Enabled variable shadowing, where existing variable may be re-defined in the same scope. For example,

```
decl a: i32
...
decl a: u8
```

### `must_declare_functions`

**Default: `false`**

Enforces the declaration of a function signature before its definition. I.e., all `func ...` must be preceded by a matching `decl func ...`.

```
decl func add(i32, i32) -> i32
...
func add(x: i32, y: i32) -> i32 { ... }
```

## Syntax

- The global scope may only consist of function definitions/declarations and data structures.
- Statements are seperated by newlines, but bracketed expressions and operators may span multiple lines e.g., `(... <eol> ...)` or `1 + <eol> 2`.
- Single-line comments have the form `// ...` and last until the end of the line.
- Multi-line comments have the form `/* ... */`.
- Identifiers start with a lowercase character, and may be followed by any number of characters, numbers, or underscores.
- Data identifiers start with an uppercase character, with the remainder identical to identifiers.

### Types

There are eight primitive integer types: `u8`, `i8`, `u16`, `i16`, `u32`, `i32`, `u64`, `i64`. They are either signed (`i...`) or unsigned (`u...`), with the number representing the number of bits. The default value is `0`.

There are two floating-point types: `f32` and `f64`. **These are yet to be implemented.**

User-defined types are also supported via the `data` keyword.

```
data <Name> {
    <field1>: <type1>[,]
    [...]
}
```

They are defined with a data identifier and contain a list of field-type pairs. Delimiting commas are optional if newlines are used.

Members may be accessed using the `.` operator:

```
data Vec { x: i32, y: i32 }

decl v: Vec
v.x // => 0
```

### Functions

The `decl func ...` compound keyword allows one to declare a function - tell the compiler about its existence - without providing a definition.
This is important as you cannot call a function which has not been declared or defined.

```
decl func <name>[<params>] [-> <return>]
```

- `<name>` is an identifier function name.
- `<params>`, if included, provides the function with parameters. If omitted, or if `()` is provided, it takes no parameters. Otherwise, it takes the form of a comma-seperated list of types (no parameter names).
- `<return>`, if provided, provides the function with a return type. If omitted, or if `()` is provided, it takes no return type. Otherwise, a type is expected.

One may define a function using the `func` keyword.

```
func <name>[<params>] [-> <return>] { <body> }
```

- `<name>` (same as above.)
- `<params>` is the same as above, expect each parameter has the form `<name>: <type>`.
- `<return>` (same as above.)
- `<body>` is a collection of statements.

Note that the same function cannot be defined more than once, but overloading is supported.

### Variables
Currently, variables are built-in numerical types, or user-defined data-structures. They are declared using the `decl` keyword, which allocates them a position on the stack.

```
decl <name>: <type>[, ...]
```

Multiple variables may be declared in one statement. Variables optionally may be shadowed, where the identifier is re-used.
