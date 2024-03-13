%include lib:macros

mov [pi], r5
print_hex r5
print_newline
print_uint r5
print_newline
print_double r5
hlt

pi: f64 3.14159
