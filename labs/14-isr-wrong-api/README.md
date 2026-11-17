# Lab 14 — wrong API in an ISR

## Why this lab

Interrupt handlers run with different rules than tasks. Calling a blocking or task-only FreeRTOS API from an ISR corrupts kernel state or trips `configASSERT`.

## What to memorize

- In an ISR: `…FromISR` variants only (`xQueueSendFromISR`, `xSemaphoreGiveFromISR`, …).
- `BaseType_t woken` out-parameter + `portYIELD_FROM_ISR(woken)` when the ISR unblocks a higher-priority task.
- Do not call `uart_puts` from the ISR if you can queue an event and print from a task (lab 10 pattern).

## Your job

Shipped `src/app.c` uses `xQueueSend` inside `BSP_IRQHandler`. Symptom: assert hang or the receiver never runs — no `DONE`.

Edit `labs/14-isr-wrong-api/src/app.c` so UART prints, in order:

1. `isr`
2. `DONE`

Keep `bsp_irq_init(5)` and the pender task looping `bsp_irq_pend()`.

## How to run

```bash
./scripts/lab.sh qemu 14
./scripts/lab.sh solution 14
```

## Done when

`./scripts/lab.sh qemu 14` shows `isr` then `DONE`.

## GDB

1. `break BSP_IRQHandler` — confirm the handler runs when the pender pend fires.
2. `break xQueueSend` / `xQueueSendFromISR` — see which path the learner build takes.
3. If hung in assert, inspect `IPSR` (non-zero in handler) and backtrace from the assert loop.
4. After fix, single-step through `portYIELD_FROM_ISR` and watch the receiver task become ready.