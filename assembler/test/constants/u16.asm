%include lib:macros

mov [num], r5
print_hex r5
hlt

num: u16 -1, 1, "Hi"