# Lab 15 — NVIC priority vs syscall mask

## Why this lab

FromISR APIs briefly raise the BASEPRI mask to `configMAX_SYSCALL_INTERRUPT_PRIORITY`. If your IRQ priority is numerically **below** that threshold (more urgent), the kernel refuses the call — another `configASSERT` class bug.

## What to memorize

- `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` is **5** here; `bsp_irq_init` takes the same 0–15 library priority.
- Rule of thumb: any ISR that calls FreeRTOS must have NVIC priority **≥ 5** (weaker or equal urgency than the syscall ceiling).
- UART on QEMU `netduinoplus2` is **USART1** on the serial you already use; this lab’s story is the synthetic `EXTI0` / `BSP_IRQHandler` path.

## Your job

Shipped `src/app.c` uses correct `FromISR` code but `bsp_irq_init(0)`. Symptom: assert in the ISR path or receiver never wakes — no `DONE`.

Change the NVIC priority so syscall APIs are legal. UART must print:

1. `isr`
2. `DONE`

## How to run

```bash
./scripts/lab.sh qemu 15
./scripts/lab.sh solution 15
```

## Done when

`./scripts/lab.sh qemu 15` shows `isr` then `DONE`.

## GDB

1. `break BSP_IRQHandler` before `xQueueSendFromISR`.
2. `print NVIC->IPR[0]` (or `x/4wx &NVIC->IPR[0]`) — EXTI0 priority byte should be **5**, not 0, after the fix.
3. `break vPortValidateInterruptPriority` (port layer) if assert fires — inspect requested priority vs `ucMaxSysCallPriority`.
4. `info symbol configMAX_SYSCALL_INTERRUPT_PRIORITY` — tie register value to the shifted mask in `FreeRTOSConfig.default.h`.