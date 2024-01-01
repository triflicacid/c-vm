; Implement OP_SUB_MEM_MEM just using OP_ADD_MEM_MEM

mov '\n', r1
; Print hex bytes of [n1] and [n2]
prh 2, n1
prc r1
prh 2, n2
prc r1
prc r1
; Negate [n2]
not 2, n2
add 2, n2, 1
; n1 = n1 + n2
add 2, n1, n2
; Print [n1]
prh 2, n1
prc r1
hlt


n1: u16 250
n2: u16 47