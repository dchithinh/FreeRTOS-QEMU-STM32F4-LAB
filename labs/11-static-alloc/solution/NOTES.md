# Lab 11 solution notes

## NULL TCB or stack

`xTaskCreateStatic` returns NULL if either the TCB pointer or stack pointer is NULL. Stack depth is in **words** (`StackType_t`), not bytes — match the array length you pass in.