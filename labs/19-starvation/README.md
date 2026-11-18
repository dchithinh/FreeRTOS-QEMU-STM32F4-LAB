# Lab 19 — starvation

## Why this lab

Equal-priority tasks only **time-slice** when `configUSE_TIME_SLICING` is 1. A CPU-bound task that never blocks can starve its peer.

## What to memorize

- `configUSE_TIME_SLICING 0` — same-priority tasks run until they block or yield.
- `configUSE_TIME_SLICING 1` — the kernel round-robins among equal ready priorities on each tick.
- Fixes: enable slicing, stagger priorities, or add `vTaskDelay` / blocking I/O in the hog task.

This lab’s `FreeRTOSConfig.h` sets `configUSE_TIME_SLICING` to `0`.

## Your job

Shipped `src/app.c` runs `spin` at priority 3 and `fair` at priority 2; `spin` never blocks. Symptom: UART never prints `fair` or `DONE`.

Edit `labs/19-starvation/src/app.c` (or `FreeRTOSConfig.h`) so UART prints:

1. `fair`
2. `DONE`

## How to run

```bash
./scripts/lab.sh qemu 19
./scripts/lab.sh solution 19
```

## Done when

`./scripts/lab.sh qemu 19` shows `fair` then `DONE`.

## GDB

1. `break vTaskSwitchContext` — with the bug, only `spin` ever runs at priority 2.
2. Inspect `pxReadyTasksLists[2]` — `fair` stays ready but never selected.
3. After fix, confirm `fair` runs and hits `uart_puts`.
4. Optionally set a watchpoint on the idle hook tick count to see time-slice behavior if you enable slicing.