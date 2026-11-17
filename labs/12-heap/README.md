# Lab 12 — heap (`heap_4`)

## Why this lab

`xTaskCreate`, queues, and timers consume bytes from the FreeRTOS heap (`pvPortMalloc` / `vPortFree` with `heap_4.c`). You need a sized `configTOTAL_HEAP_SIZE` and must handle exhaustion.

## What to memorize

- `pvPortMalloc` / `vPortFree`; heap owned by the kernel, not libc `malloc`.
- `vTaskStartScheduler` already allocates the **idle** and **timer daemon** tasks from the heap (unless fully static hooks cover them).
- `configUSE_MALLOC_FAILED_HOOK` — failed allocations call `vApplicationMallocFailedHook`, which on this BSP prints `HOOK MALLOC_FAILED` and spins (see `bsp/hooks.c`).
- Prefer `uart_puts` for lab output; libc printf can use `_sbrk` and confuse debugging when heap is tight.

## Your job

Implement `labs/12-heap/src/app.c` so UART prints, in order:

1. `alloc_ok`
2. `DONE`

Requirements:

- In a task, `pvPortMalloc(16)`; on success print `alloc_ok`, then `vPortFree`.
- Do not trigger the malloc-failed hook on the success path.
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 12
./scripts/lab.sh check 12
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 12` exits 0 and `./scripts/lab.sh qemu 12` shows the same two lines (modulo QEMU noise before the app runs).