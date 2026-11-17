# Lab 05 — mutex

## Why this lab

Mutexes protect shared resources so only one task enters a critical section at a time. Unlike a counting semaphore, a mutex encodes **ownership** and is the right primitive for mutual exclusion around data structures or drivers.

## What to memorize

- `xSemaphoreCreateMutex()` vs `xSemaphoreCreateBinary()`: mutex adds ownership; binary semaphore is for signaling (not a drop-in substitute for protecting shared state).
- `xSemaphoreTake` / `xSemaphoreGive` on a mutex: only the owning task should give; take may block until available.
- **Do not** use the mutex API from an ISR — use a FromISR primitive or defer work to a task.
- **Recursive mutex** (`xSemaphoreCreateRecursiveMutex`): the same task may take nested times; each take needs a matching give. Useful for re-entrant call paths; ordinary mutex deadlocks if the holder takes again.

## Your job

Implement `labs/05-mutex/src/app.c` so UART prints, in order:

1. `locked`
2. `unlocked`
3. `DONE`

Requirements:

- One mutex shared by **two** tasks; each takes the mutex, prints its line, then gives the mutex.
- Higher-priority locker runs first so output is deterministic (`locked` before `unlocked`).
- A third task prints `DONE` after both lockers finish.
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 05
./scripts/lab.sh check 05
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 05` exits 0 and `./scripts/lab.sh qemu 05` shows the same three lines (modulo QEMU noise before the app runs).