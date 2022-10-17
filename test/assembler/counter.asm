mov 0, r1 ; Current value
mov 10, r3
loop:
cmp r1, Fh
jgt end ; if (r1 > 10) break
prh r1
add r1, 1 ; r1++
prc r3
jmp loop
end:
hlt