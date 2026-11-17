# Lab 04 — semaphores

## Why this lab

Semaphores count resources or signal events. A **counting** semaphore tracks how many units are available; a **binary** semaphore is the special case max count 1 (often used for signaling, not substitution for a mutex).

## What to memorize

- `xSemaphoreCreateCounting(max, initial)`: `max` caps the count; `initial` is starting availability.
- `xSemaphoreCreateBinary()` starts empty (typical signal pattern) unless you `Give` before use.
- `xSemaphoreTake` / `xSemaphoreGive` decrement/increment the count; take blocks when count is zero (unless timeout is zero).
- Mutexes add ownership rules; binary semaphores do not — use mutex for protecting shared data.

## Your job

Implement `labs/04-semaphores/src/app.c` so UART prints, in order:

1. `took`
2. `took`
3. `DONE`

Requirements:

- Counting semaphore with maximum count **2**; preload **two** tokens before `vTaskStartScheduler()` (or equivalent so both takers can proceed).
- Two tasks each `Take` once and print `took`.
- A third task prints `DONE` after both takes have had time to run.
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 04
./scripts/lab.sh check 04
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 04` exits 0 and `./scripts/lab.sh qemu 04` shows the same three lines (modulo QEMU noise before the app runs).