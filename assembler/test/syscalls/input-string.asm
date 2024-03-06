; Prompt for string
mov 16, r0
mov user_input, r1
mov 64, r2
syscall

; Print result
mov 6, r0
mov user_input, r1
syscall

; Print length (stored in r3)
mov 0, r0
mov r3, r1
syscall

hlt

user_input: u8
