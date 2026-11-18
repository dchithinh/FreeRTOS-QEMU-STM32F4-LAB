# Lab 24 — FreeRTOSConfig

## Why this lab

Every knob in `bsp/include/FreeRTOSConfig.default.h` changes kernel behaviour. `configTICK_RATE_HZ` is the SysTick rate: it scales `pdMS_TO_TICKS()` and defines how fast `xTickCount` advances.

## What to memorize

Read `FreeRTOSConfig.default.h` line by line. Focus on:

- `configTICK_RATE_HZ`, `configCPU_CLOCK_HZ`, heap and stack sizes
- Preemption, time slicing, idle/timer task priorities
- `configMAX_SYSCALL_INTERRUPT_PRIORITY` vs NVIC priorities (see lab 15)
- `configASSERT`, stack overflow, malloc failed hooks

Kernel sources for tick time: `xPortSysTickHandler()` in `third_party/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c` calls `xTaskIncrementTick()`. The global tick counter `xTickCount` lives in `third_party/FreeRTOS-Kernel/tasks.c`.

## Your job

This lab’s `FreeRTOSConfig.h` sets `configTICK_RATE_HZ` to `100` instead of `1000`. The demo delays ten times for 10 ms each, then prints `ticks=…` and `DONE`. Wall time is ~100 ms, but the printed tick count is far too low.

Fix `labs/24-freertos-config/FreeRTOSConfig.h` so elapsed `ticks=` is about `100` (not `10`) and `DONE` still appears.

## How to run

```bash
./scripts/lab.sh qemu 24
./scripts/lab.sh solution 24
```

## Done when

`./scripts/lab.sh qemu 24` prints `ticks=100` (±1) then `DONE`, matching solution timing.

## GDB

1. `./scripts/lab.sh gdb 24` — break in `demo_task` before the final `ticks=` print.
2. `print/x xTickCount` after the delay loop — compare learner vs solution firmware.
3. `break xPortSysTickHandler` — single-step into `xTaskIncrementTick`.
4. `break vPortSetupTimerInterrupt` — see how reload value follows `configTICK_RATE_HZ`.