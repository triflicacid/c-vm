# Syscalls

Syscalls, or system calls, are built-in operations which may be invoked by the user.
Syscalls are invoked using the `syscall` instruction. The operation code is located in `r0`. Subsequenst arguments, if any, are sequentially located in `r1`, `r2`, ...

| Service                | Code | Arguments | Description                                         |
|------------------------|------|-----------|-----------------------------------------------------|
| Exit                   | -1   | code      | Exit with the given code.                           |
| Print integer          | 0    | data      | Print word as integer.                              |
| Print unsigned integer | 1    | data      | Print word as unsigned integer.                     |
| Print hexadecimal      | 2    | data      | Print word as hexadecimal.                          |
| Print float            | 3    | data      | Print word as float.                                |
| Print double           | 4    | data      | Print word as double.                               |
| Print character        | 5    | data      | Print word as a sequence of characters.             |
| Print string           | 6    | len, addr | Print `len` bytes starting from `addr` as a string. |
