# Registers

Registers are single-word storage containers. They are represented by an array. Special registers' indexes into said array are given by constants in `cpu.h`. Any register without a special user is considered "general purpose".

- `REG_FLAG`. This register is used to store additional information relating to an instruction which has been executed.
  - Stores quotient after integer division
  - Stores integer overflow from the addition of n-byte buffers
  - Stores extra error information when an error is signalled.
- `REG_CMP`. This register stores the result of a `CMP` instruction, and is used by `JMP` variants for branching.
- `REG_ERR`. This register stores the current error code of the CPU. Error constants are defined in `cpu/err.h`. Additional error information is stored in `REG_FLAG`.
- `REG_IP`. Stores the Instruction Pointer - the address of the next instruction to be fetched and executed. When an error occurs, may point to the location in memory which triggered the error.
- `REG_SP`. Stores the Stack Pointer - points to the address of the top of the stack (the next empty address).
- `REG_SSIZE`. Stores the size (in bytes) of the stack.
- `REG_FP`. Stores the Frame Pointer - points to the top of the last saved stack frame.
- `REG_FSIZE`. Stores the current stack frames' size.

Note that the reference symbols of a register is defined by `REG_<name>_SYM`