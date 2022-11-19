mov 10, r5
mov 0, r1
prb ccr ; initial
prc r5
neg r1 ; Test normal
prb ccr ; Expect: 0100
prc r5
mov32 F, r1
negf32 r1 ; Test float
prb ccr ; Expect: 0100
prc r5
mov D, r1
negf64 r1 ; Test double
prb ccr ; Expect: 0100
prc r5
hlt

F: f32 0
D: f64 0