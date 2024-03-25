mov [data], r5
psh r5
pop r1
mov 0, r0
syscall
hlt

data: u8 42
