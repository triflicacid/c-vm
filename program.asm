mov 0, r0
mov [num], r1
syscall
hlt

num: u8 123
