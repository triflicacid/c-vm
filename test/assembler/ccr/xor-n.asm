mov 10, r5
mov 5, r1
mov -8, r2
prb ccr ; initial
prc r5
xor r1, -255 ; Test reg_lit
prb ccr ; Expect: 1000
prc r5
mov 5, r1
xor r1, r2 ; Test reg_reg
prb ccr ; Expect: 1000
prc r5
xor 9, V1, V2 ; Test mem_mem
prb ccr ; Expect: 1000
prc r5
hlt

V1: u8 0 0 0 0 0 3 0 0 88h
V2: u8 0 0 0 0 0 3 0 0 08h