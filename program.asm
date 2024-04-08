mov 10, r1
mov 9, r2
cmp r1, r2
jle if
else:
mov 0, r1
jmp end
if:
mov 1, r1
end:
mov 0, r0
syscall
hlt
