# Lab 20 — heap leak

## Why this lab

Dynamic objects (queues, tasks, mutexes) draw from `configTOTAL_HEAP_SIZE`. Leaking handles after `xQueueCreate` eventually trips `vApplicationMallocFailedHook`.

## What to memorize

- `xQueueCreate` allocates queue storage + control block from the FreeRTOS heap.
- `vQueueDelete` returns that memory when nothing references the queue.
- This lab uses a **8 KiB** heap overlay in `FreeRTOSConfig.h` so failure happens quickly in QEMU.

## Your job

Shipped `src/app.c` loops `xQueueCreate` and never calls `vQueueDelete`. Symptom:

1. `HOOK MALLOC_FAILED`
2. hang (no `DONE`)

Edit `labs/20-heap-leak/src/app.c` so UART prints `DONE` without tripping the hook.

## How to run

```bash
./scripts/lab.sh qemu 20
./scripts/lab.sh solution 20
```

## Done when

`./scripts/lab.sh qemu 20` prints `DONE` and does **not** print `HOOK MALLOC_FAILED`.

## GDB

1. `break vApplicationMallocFailedHook` — when the bug is present, catch the exhausted heap.
2. `break pvPortMalloc` / `vPortFree` — count allocations without matching frees in `src`.
3. `xPortGetFreeHeapSize` from gdb if symbols are available — watch it fall each loop iteration.
4. After fix, confirm `vQueueDelete` (or reuse) keeps the heap above zero through `DONE`.