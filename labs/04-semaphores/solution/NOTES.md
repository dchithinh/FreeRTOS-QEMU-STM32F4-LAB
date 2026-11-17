# Lab 04 solution notes

## Preloading tokens

`xSemaphoreCreateCounting(2, 0)` then two `xSemaphoreGive` calls in `main` before the scheduler starts models “two resources ready” without a giver task.

## Order of `took` lines

`t1` runs at priority 3 and `t2` at priority 2 so each `uart_puts` completes without interleaving (see `bsp/uart.c` critical section). Either taker may print first depending on ready order; `expected.log` is two `took` lines then `DONE`. Both takes finish before `done_task`’s 100 ms delay.

## Why not binary?

Two concurrent takes need max count 2; a binary semaphore would block the second taker until another give.