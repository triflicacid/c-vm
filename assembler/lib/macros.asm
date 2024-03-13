%include syscalls.asm

%macro _syscall_0 op
    mov op, r0
    syscall
%end

%macro _syscall_1 reg op
    mov reg, r1
    mov op, r0
    syscall
%end

%macro _syscall_2 reg1 reg2 op
    mov reg1, r1
    mov reg2, r2
    mov op, r0
    syscall
%end

%macro print_int reg
    _syscall_1 reg, SYSCALL_PRINT_INT
%end

%macro input_int
    _syscall_0 SYSCALL_INPUT_INT
%end

%macro print_uint reg
    _syscall_1 reg, SYSCALL_PRINT_UINT
%end

%macro input_uint
    _syscall_0 SYSCALL_INPUT_UINT
%end

%macro print_hex reg
    _syscall_1 reg, SYSCALL_PRINT_HEX
%end

%macro input_hex
    _syscall_0 SYSCALL_INPUT_HEX
%end

%macro print_float reg
    _syscall_1 reg, SYSCALL_PRINT_FLOAT
%end

%macro input_float
    _syscall_0 SYSCALL_INPUT_FLOAT
%end

%macro print_double reg
    _syscall_1 reg, SYSCALL_PRINT_DOUBLE
%end

%macro input_double
    _syscall_0 SYSCALL_INPUT_DOUBLE
%end

%macro print_char reg
    _syscall_1 reg, SYSCALL_PRINT_CHAR
%end

%macro input_char
    _syscall_0 SYSCALL_INPUT_CHAR
%end

%macro print_string str_addr len
    _syscall_2 str_addr, len, SYSCALL_PRINT_STRING
%end

%macro input_string str_addr max_len
    _syscall_2 str_addr, max_len, SYSCALL_INPUT_STRING
%end

%macro print_registers
    _syscall_0 SYSCALL_PRINT_REGISTERS
%end

%macro print_memory addr len
    _syscall_2 addr, len, SYSCALL_PRINT_MEMORY
%end

%macro print_newline
    mov SYSCALL_PRINT_CHAR, r0
    mov '\n', r1
    syscall
%end
