mov 6, r0
mov string1, r1
mov 0, r2
syscall

mov string2, r1
syscall

hlt

.section data
string1: u8 "Hello, " 0
string2: u8 "World!" 0
