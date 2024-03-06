nop
jmp label ; Label is not defined - cache
label: ; Define label to be +12
       ; Replace cached reference on line 2: `jmp label` -> `jmp 12`
jmp label ; Label is defined - jmp 12
label: ; Re-define label to be +22
jmp label ; Label is defined - jmp 22
hlt