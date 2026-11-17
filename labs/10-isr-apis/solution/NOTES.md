# Lab 10 solution notes

## UART from ISR

`uart_puts` is not ISR-safe here. The handler only posts to a queue; the receiver task prints. That pattern avoids reentrancy issues and keeps ISR runtime short.