# Lab 22 solution notes

Two tasks at the same priority preempt each other; `counter++` is load–add–store in C and can lose updates without mutual exclusion.

`uart_puts` uses critical sections in the BSP, but the race is in the increment loop — printing happens only after the tasks finish.