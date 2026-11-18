# Lab 26 — SysTick and the tick count

## Why this lab

Hardware SysTick drives the RTOS clock. Each interrupt bumps `xTickCount` through `xTaskIncrementTick()`; delays and timeouts are measured in ticks.

## What to memorize

- `xPortSysTickHandler()` in `third_party/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c` (often aliased to `SysTick_Handler`).
- When `xTaskIncrementTick()` returns `pdTRUE`, the port pends `PendSV` for a deferred context switch.
- `vTaskDelay(n)` blocks until at least `n` tick periods elapse (not wall ms unless `configTICK_RATE_HZ` matches your mental model).

## Your job

Edit `labs/26-systick-tick/src/app.c`. A low-priority task should:

1. `vTaskDelay(5)`
2. Print `tick <n>` where `<n>` is how many ticks elapsed across `vTaskDelay(5)` (expect `5`)
3. Print `DONE`

## How to run

```bash
./scripts/lab.sh qemu 26
./scripts/lab.sh solution 26
```

## Done when

`./scripts/lab.sh qemu 26` shows `tick 5` (±1) then `DONE`.

## GDB

1. `break xPortSysTickHandler` — single-step into `xTaskIncrementTick`; watch `xTickCount`.
2. `break SysTick_Handler` if the symbol name is used in your map file.
3. Set a breakpoint in `tick_task` after `vTaskDelay` returns; confirm tick value.
4. When delayed, inspect the blocked task’s delay list in `tasks.c` (`xDelayedTaskList1`).