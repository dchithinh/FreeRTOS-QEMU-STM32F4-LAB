# Lab 04 solution notes

## Preloading tokens

`xSemaphoreCreateCounting(2, 0)` then two `xSemaphoreGive` calls in `main` before the scheduler starts models “two resources ready” without a giver task.

## Order of `took` lines

Both taker tasks share the same priority; either may print first. `expected.log` lists two `took` lines then `DONE`; the checker compares the full UART capture as a set of lines in order — with equal priority and preload, both takes complete before `done_task`’s 100 ms delay elapses.

## Why not binary?

Two concurrent takes need max count 2; a binary semaphore would block the second taker until another give.