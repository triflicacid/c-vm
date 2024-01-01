mov Ah, r1
mov 3.14, r2
prh r2 ; Print as hex
prc r1
prd r2 ; Print as double
prc r1
pri r2 ; Print as int
prc r1
cf64i64 r2 ; double -> int
pri r2 ; Print as int
prc r1
hlt