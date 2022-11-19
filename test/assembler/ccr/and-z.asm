mov 10, r5
mov 0, r1
mov -1, r2
prb ccr ; initial
prc r5
and r1, 0 ; Test reg_lit
prb ccr ; Expect: 0100
prc r5
and r1, 0 ; Test reg_reg
prb ccr ; Expect: 0100
prc r5
and 9, V1, V2 ; Test mem_mem
prb ccr ; Expect: 0100
prc r5
hlt

V1: u8 01h 02h 03h 04h 05h 06h 07h 08h 09h
V2: u8 10h 20h 30h 40h 50h 60h 70h 80h 90h