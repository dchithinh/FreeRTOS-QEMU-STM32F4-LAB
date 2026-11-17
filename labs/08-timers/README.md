# Lab 08 — timers

## Why this lab

FreeRTOS software timers fire periodically or once after a delay. Callbacks run in the timer **daemon** task, not in your application task context — that shapes what you can safely do inside the callback.

## What to memorize

- `xTimerCreate`, `xTimerStart`, `xTimerStop`, `xTimerReset`; period in tick counts (`pdMS_TO_TICKS`).
- Auto-reload (`pdTRUE`) vs one-shot (`pdFALSE`).
- Timer callbacks execute in the timer daemon task — **do not block** (no `vTaskDelay`, no take with non-zero timeout).
- `xTimerStop` / `xTimerReset` from a callback typically use command queue block time **0** (do not wait on the daemon while you are the daemon).
- `configUSE_TIMERS` and `configTIMER_TASK_PRIORITY` must be set; starting the scheduler creates the daemon.

## Your job

Implement `labs/08-timers/src/app.c` so UART prints, in order:

1. `tick`
2. `tick`
3. `DONE`

Requirements:

- Auto-reload software timer with **50 ms** period.
- Callback prints `tick` each expiration; after **2** ticks, stop the timer and print `DONE` from the callback.
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 08
./scripts/lab.sh check 08
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 08` exits 0 and `./scripts/lab.sh qemu 08` shows the same three lines (modulo QEMU noise before the app runs).