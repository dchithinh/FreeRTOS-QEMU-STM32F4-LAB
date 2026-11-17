# Lab 10 — ISR-safe APIs

## Why this lab

Interrupt handlers cannot call blocking task APIs. FreeRTOS provides `FromISR` variants and `portYIELD_FROM_ISR` so an ISR can hand off work to a task safely.

## What to memorize

- Task APIs (`xQueueSend`, `xSemaphoreGive`, …) vs ISR APIs (`…FromISR`).
- `BaseType_t woken` + `portYIELD_FROM_ISR(woken)` when the ISR unblocks a higher-priority task.
- **Do not print from the ISR** if you can avoid it — queue an event and let a task log to UART.
- NVIC priority must be **at or below** `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (numerically **≥** 5 on this BSP) for `FromISR` calls to work.

## Your job

Implement `labs/10-isr-apis/src/app.c` so UART prints, in order:

1. `isr`
2. `DONE`

Requirements:

- `uart_init()` then `bsp_irq_init(5)` before the scheduler starts.
- A receiver task blocks on a queue; a pender task loops `bsp_irq_pend()` so QEMU enters `BSP_IRQHandler`.
- In the handler, use `xQueueSendFromISR` (not `xQueueSend`); the receiver prints `isr` and `DONE`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 10
./scripts/lab.sh check 10
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 10` exits 0 and `./scripts/lab.sh qemu 10` shows the same two lines (modulo QEMU noise before the app runs).