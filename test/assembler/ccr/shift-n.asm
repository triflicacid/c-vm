mov 10, r5
mov 1, r1
mov 63, r2

prb ccr ; initial
prc r5
sll r1, 63 ; Test reg_lit
prb ccr ; Expect: 1000
prc r5
mov 1, r1
sll r1, r2 ; Test reg_reg
prb ccr ; Expect: 1000
prc r5

slr r1, 0 ; Test reg_lit
prb ccr ; Expect: 1000
prc r5
mov 0, r2
slr r1, r2 ; Test reg_reg
prb ccr ; Expect: 1000
prc r5

sar r1, 1 ; Test reg_lit
prb ccr ; Expect: 1000
prc r5
sar r1, r2 ; Test reg_reg
prb ccr ; Expect: 1000
prc r5

hlt