%macro print_registers
    mov 100, r0
    syscall
%end

%macro swap ra rb
    mov ra, r0
    mov rb, ra
    mov r0, rb
%end

mov 1, r1
mov 2, r2
print_registers
swap r1, r2
print_registers
hlt
