mov 10, r5
mov 5, r1
mov -8, r2
prb ccr ; initial
prc r5
or r1, -1 ; Test reg_lit
prb ccr ; Expect: 1000
mov 5, r1
prc r5
or r1, r2 ; Test reg_reg
prb ccr ; Expect: 1000
prc r5
or 9, V1, V2 ; Test mem_mem
prb ccr ; Expect: 1000
prc r5
hlt

V1: u8 0 0 0 0 0 0 0 0 80h
V2: u8 0 0 0 0 0 0 0 0 1