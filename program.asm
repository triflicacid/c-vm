mov 6, r0
mov string, r1
xor r2, r2
syscall
hlt

string: u8 "Hello World!\n\0"
