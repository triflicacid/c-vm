%include lib:macros
%define upper_bound 10

mov 0, r4  ; Sum
mov 1, r5  ; Current number
mov upper_bound, r6  ; Limit

loop:
add r4, r5
cmp r5, r6
add r5, 1
jlt loop

;add r4, r6

mov 6, r0
mov string, r1
mov 0, r2
syscall

mov 0, r0
mov r4, r1
syscall

hlt

string: u8 "Sum of first upper_bound natural numbers: " 0
ieq: u8 "i=" 0
