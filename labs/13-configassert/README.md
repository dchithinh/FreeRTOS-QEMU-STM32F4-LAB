# Lab 13 — configASSERT

## Why this lab

FreeRTOS validates preconditions inside the kernel. A bad parameter (NULL handle, zero-length queue, calling the wrong API context) hits `configASSERT` and stops — better than corrupting a list or deadlocking silently.

## What to memorize

- `configASSERT(x)` in this repo: on failure, `taskDISABLE_INTERRUPTS()` then infinite loop (see `FreeRTOSConfig.default.h`).
- Common triggers: NULL `QueueHandle_t`, `uxQueueLength == 0` in `xQueueCreate`, mutex/semaphore API misuse.
- Assert is not a substitute for checking return codes — use asserts for “must never happen” in debug builds.

## Your job

Shipped `src/app.c` calls `xQueueReceive(NULL, …)`. UART hangs with no `DONE`.

Edit `labs/13-configassert/src/app.c` so UART prints, in order:

1. `alive`
2. `DONE`

Use the real queue handle and legal API arguments only.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 13
./scripts/lab.sh solution 13
```

There is no `check` target for Phase 2 labs.

## Done when

`./scripts/lab.sh qemu 13` shows `alive` then `DONE` (not a silent hang). Compare with `./scripts/lab.sh solution 13` if stuck.

## GDB

1. `./scripts/lab.sh gdb 13` — break in `worker_task` before `xQueueReceive`.
2. `break xQueueReceive` — step in; note the `configASSERT` on a NULL handle.
3. If hung, PC is often in the `for(;;)` inside the `configASSERT` macro after `taskDISABLE_INTERRUPTS`.
4. Inspect `msg_q` — it is non-NULL while the bad call still passes `NULL`.