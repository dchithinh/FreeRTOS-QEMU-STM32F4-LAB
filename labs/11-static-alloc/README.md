# Lab 11 — static allocation

## Why this lab

Some systems ban heap use after boot. `xTaskCreateStatic` lets you supply the TCB and stack buffers yourself so task control blocks do not come from `pvPortMalloc`.

## What to memorize

- `xTaskCreateStatic(fn, name, stack_depth, param, priority, stack_buffer, tcb_buffer)`.
- You must provide a `StaticTask_t` and a `StackType_t[]` sized to `stack_depth` words — **NULL buffers fail**.
- `configSUPPORT_STATIC_ALLOCATION` must be 1 (this lab's `FreeRTOSConfig.h` sets it).
- The idle (and timer) tasks can also use static buffers via `vApplicationGetIdleTaskMemory` / `vApplicationGetTimerTaskMemory` in `bsp/hooks.c`.

## Your job

Implement `labs/11-static-alloc/src/app.c` so UART prints, in order:

1. `static`
2. `DONE`

Requirements:

- Create the worker with `xTaskCreateStatic`, not `xTaskCreate`.
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 11
./scripts/lab.sh check 11
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 11` exits 0 and `./scripts/lab.sh qemu 11` shows the same two lines (modulo QEMU noise before the app runs).