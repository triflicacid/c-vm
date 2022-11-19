mov 10, r5
mov -1, r1
prb ccr ; initial
prc r5
not r1 ; Test reg
prb ccr ; Expect: 0100
prc r5
not 9, V1 ; Test mem
prb ccr ; Expect: 0100
prc r5
hlt

V1: u8 FFh FFh FFh FFh FFh FFh FFh FFh FFh