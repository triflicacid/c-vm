mov 42, r1

psh 3
psh 4
psh 16

cal func
hlt

func:
    psh 10
    mov 69, r1
    ret
