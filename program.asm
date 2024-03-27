mov 6, r0
mov string, r1
mov 0, r2
syscall
hlt

.section data
string: u8 "Hello", 0
