# Lab 09 — stream and message buffers

## Why this lab

For byte-stream or fixed-size message I/O between tasks, FreeRTOS stream buffers offer lighter memcpy-style channels than full queues. Message buffers are stream buffers with a size prefix per write.

## What to memorize

- `xStreamBufferCreate(size, trigger_level)`; `xStreamBufferSend` / `xStreamBufferReceive`.
- **Single writer and single reader** at a time — the design is not safe with multiple writers or multiple readers without external serialization.
- Trigger level wakes the reader when at least that many bytes are present.
- Distinct from queues: streams move raw bytes; queues copy typed items with depth counting.

## Your job

Implement `labs/09-stream-message-buffers/src/app.c` so UART prints, in order:

1. `abc`
2. `DONE`

Requirements:

- One stream buffer; one writer task sends the three bytes `abc`; one reader receives and prints them.
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 09
./scripts/lab.sh check 09
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 09` exits 0 and `./scripts/lab.sh qemu 09` shows the same two lines (modulo QEMU noise before the app runs).