mov 4, r0
mov 3.14, r1
syscall

mov 5, r0
mov '\n', r1
syscall

mov 6, r0
mov 13, r1
mov string, r2
prr
syscall

hlt

string: u8 "Hello World!\n"
