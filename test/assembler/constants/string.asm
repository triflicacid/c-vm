mov string, r1
mov br, r2
prh r1
prc r2
prc r1
prc r2
hlt

string: u8 "Hi!"
br: u8 '\n'