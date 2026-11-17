# Lab 02 — timing

## Why this lab

Real firmware waits on clocks: sensor periods, timeouts, and phased workers. FreeRTOS express time in **ticks**; you must know how to block for a duration versus waking on a fixed schedule so jitter does not accumulate.

## What to memorize

- `vTaskDelay(ticks)` blocks **relative** to “now”: every wake time drifts if work before the delay varies.
- `vTaskDelayUntil(&last_wake, period)` blocks until the **next absolute** deadline; the period stays stable across loops.
- `pdMS_TO_TICKS(ms)` converts milliseconds to ticks using `configTICK_RATE_HZ` (this repo: **1000 Hz** → 1 tick = 1 ms).
- A tick is the scheduler’s time quantum; `portTICK_PERIOD_MS` is `1000 / configTICK_RATE_HZ`.

## Your job

Implement `labs/02-timing/src/app.c` so UART prints, in order:

1. `a`
2. `b`
3. `a`
4. `b`
5. `DONE`

Requirements:

- Two tasks each print their letter **twice** on a **50 ms** cadence, with task `b` **25 ms** behind task `a`.
- A third task prints `DONE` after enough time has passed for the pattern to finish (use `vTaskDelay`).
- Call `uart_init()` before `vTaskStartScheduler()`.

The solution uses `vTaskDelayUntil` for the periodic prints; you may start with `vTaskDelay` and refactor once the pattern is right.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 02
./scripts/lab.sh check 02
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 02` exits 0 and `./scripts/lab.sh qemu 02` shows the same five lines (modulo QEMU noise before the app runs).