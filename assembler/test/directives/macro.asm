%macro prch reg
    mov reg, r1
    mov 5, r0
    syscall
%end

mov "Hi!\n", r0
prch r0
hlt
