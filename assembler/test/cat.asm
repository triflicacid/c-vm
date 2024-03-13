%include ../lib/syscall_macros.asm

main:
input_string string, 1024
print_string string, r3
print_char '\n'
jmp main

string: u8
