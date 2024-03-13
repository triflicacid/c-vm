%include lib:macros

mov '\n', r5
print_memory string, 4
print_char r5
print_string string, 0
print_char r5
hlt

string: u8 "Hi!", 0
