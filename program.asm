;mov [data], r5
;psh r5
;pop r1
;mov 0, r0
;syscall
;hlt
;
;.section data
;data: u8 42

mov 6, r0
mov string, r1
mov 0, r2
syscall
hlt

.section data
string: u8 "Hello", 0
