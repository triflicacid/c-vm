%include lib:macros

mov "Hello\n", r5
print_char r5
hlt
%stop
del r1 ; <-- This is never reached
