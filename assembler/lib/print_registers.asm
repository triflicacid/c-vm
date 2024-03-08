; Macro definitions to easily print register's contents

%include syscalls.asm

%macro _syscall_1 reg op
    mov reg, r1
    mov op, r0
    syscall
%end

%macro print_int reg
    _syscall_1 reg, SYSCALL_PRINT_INT
%end

%macro print_uint reg
    _syscall_1 reg, SYSCALL_PRINT_UINT
%end

%macro print_hex reg
    _syscall_1 reg, 2
%end

%macro print_float reg
    _syscall_1 reg, 3
%end

%macro print_double reg
    _syscall_1 reg, 4
%end

%macro print_chars reg
    _syscall_1 reg, 5
%end

%macro print_string str_addr len
    mov 6, r0
    mov str_addr, r1
    mov len, r2
    syscall
%end
