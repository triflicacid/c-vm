mov 10, r5
mov 0, r1
mov 0, r2
prb ccr ; initial
prc r5
xor r1, 0 ; Test reg_lit
prb ccr ; Expect: 0100
prc r5
xor r1, r2 ; Test reg_reg
prb ccr ; Expect: 0100
prc r5
xor 9, V1, V2 ; Test mem_mem
prb ccr ; Expect: 0100
prc r5
hlt

V1: u8 0 0 0 0 0 3 0 0 0
V2: u8 0 0 0 0 0 3 0 0 0