# Errors

Errors are thrown when something irreparably goes wrong.

## Machine Code Execution

These are errors which are thrown during machine code execution. The error code is stored in `REG_ERR` and associated error data is stored in `REG_FLAG`.

| error             | REG_FLAG        | Description                                      |
|-------------------|-----------------|--------------------------------------------------|
| `ERR_NONE`        | *N/A*           | No error                                         |
| `ERR_MEMOOB`      | Memory address  | Attempted to access out-of-bounds memory address |
| `ERR_REG`         | Register offset | Attempted to access an illegal register          |
| `ERR_UNINST`      | Opcode          | Encountered illegal opcode during FE-cycle       |
| `ERR_STACK_UFLOW` | *N/A*           | Attempted to POP of an empty stack               |
| `ERR_STACK_OFLOW` | Memory address  | Stack has overflown - size exceeds capacity      |
