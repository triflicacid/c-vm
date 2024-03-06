# Syscalls

Syscalls, or system calls, are built-in operations which may be invoked by the user.
Syscalls are invoked using the `syscall` instruction. The operation code is located in `r0`. Subsequenst arguments, if any, are sequentially located in `r1`, `r2`, ...

| Service                   | Code | Arguments | Description                                                                                         |
|---------------------------|------|-----------|-----------------------------------------------------------------------------------------------------|
| Exit                      | -1   |           | Exit program.                                                                                       |
| Print integer             | 0    | data      | Print word as integer.                                                                              |
| Print unsigned integer    | 1    | data      | Print word as unsigned integer.                                                                     |
| Print hexadecimal         | 2    | data      | Print word as hexadecimal.                                                                          |
| Print float               | 3    | data      | Print word as float.                                                                                |
| Print double              | 4    | data      | Print word as double.                                                                               |
| Print character           | 5    | data      | Print word as a sequence of characters up to `\0`.                                                  |
| Print string              | 6    | addr, len | Print `len` bytes starting from `addr` as a string. If `len` is 0, treat string as null-terminated. |
| [*Debug*] Print registers | 100  |           | Prints contents of registers                                                                        |
| [*Debug*] Print memory    | 101  | addr, len | Print memory from `addr`.                                                                           |
| [*Debug*] Print stack     | 102  |           | Print contents of stack.                                                                            |