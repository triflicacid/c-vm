mov 101, r0
mov string, r1
mov 64, r2
syscall
hlt

string: u8 "Hello, world\n\0"
pi: f64 3.14159
