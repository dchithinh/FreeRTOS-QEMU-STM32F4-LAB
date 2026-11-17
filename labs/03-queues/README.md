# Lab 03 — queues

## Why this lab

Tasks often exchange data safely without sharing mutable globals. FreeRTOS queues copy items between sender and receiver; you choose length, item size, and how long to block when the queue is full or empty.

## What to memorize

- `xQueueCreate(length, item_size)`: storage is roughly **length × item_size** bytes (plus kernel overhead).
- `xQueueSend` / `xQueueReceive` **copy** the item by value unless you use a queue-of-pointers pattern deliberately.
- Block time (`portMAX_DELAY`, `0`, or ticks) controls whether the call waits, fails immediately, or times out.
- ISR variants (`FromISR`) exist; this lab uses task-context APIs only.

## Your job

Implement `labs/03-queues/src/app.c` so UART prints, in order:

1. `42`
2. `DONE`

Requirements:

- One queue: length **1**, each item `sizeof(uint32_t)`.
- A producer task sends the value **42**; a consumer receives it and prints the decimal value on UART (verify it is `42`).
- After printing the number, print `DONE` (same task or a follow-up step is fine).
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 03
./scripts/lab.sh check 03
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 03` exits 0 and `./scripts/lab.sh qemu 03` shows the same two lines (modulo QEMU noise before the app runs).