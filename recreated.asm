mov 6 r0 
mov data0 r1 
mov 0 r2 
syscall 
hlt 
data0: u8 "Hello\0"
