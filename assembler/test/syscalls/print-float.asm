mov 3, r0
mov [m_data], r1
syscall
hlt

m_data: f32 3.141
