; Single literal
mov 'A', r1
prc r1

; Multiple literals
mov 'H' 'e' 'l' 'l' 'o' '!', r1
prc r1

; Escape sequences
mov '\d65' '\t' '\x41', r1
prc r1

hlt