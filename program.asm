mov 6, r0
mov [data], r1
xor r2, r2
syscall
hlt

data: u8 "Hello World", 0
