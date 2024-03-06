mov 3, r0
mov [data], r1
syscall
hlt

data: f32 3.141
