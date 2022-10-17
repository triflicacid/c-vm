nop
jmp label ; Label is not defined - cache
label: ; Define label to be +12
jmp label ; Label is defined - jmp [12]
label: ; Re-define label to be +22
jmp label ; Label is defined - jmp [22]
hlt
; End of program. Replaced cache label with its offset: `jmp label` -> `jmp [22]`