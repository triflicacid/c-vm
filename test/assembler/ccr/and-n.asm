mov 10, r5
mov -9_223_372_036_854_775_808, r1
mov -1, r2
prb ccr ; initial
prc r5
and r1, -8 ; Test reg_lit
prb ccr ; Expect: 1000
prc r5
and r1, -1 ; Test reg_reg
prb ccr ; Expect: 1000
prc r5
and 9, V1, V2 ; Test mem_mem
prb ccr ; Expect: 1000
prc r5
hlt

V1: u8 01 02 03 04 05 06 07 08 FFh
V2: u8 10 20 30 40 50 60 70 80 80h