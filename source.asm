mov '\n', r1
; Print hex bytes of [n1] and [n2]
prh 2, n1
prc r1
prh 2, n2
prc r1
prc r1
; n1 = n1 - n2
sub 2, n1, n2
; Print [n1]
prh 2, n1
prc r1
hlt


 n1: u16 250
 n2: u16 47