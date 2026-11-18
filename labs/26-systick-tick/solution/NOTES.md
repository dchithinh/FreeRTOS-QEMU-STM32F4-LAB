# Lab 26 solution notes

## What the demo proves

`tick_task` samples `xTaskGetTickCount()` before and after `vTaskDelay(5)`. The printed delta is the number of **scheduler ticks**, not milliseconds, unless you mentally convert using `configTICK_RATE_HZ` (1000 Hz here).

## Why ±1 is OK

Tick boundaries and the moment `xTaskGetTickCount()` runs relative to the next SysTick edge can make the elapsed count `4` or `6` on rare runs; `5` is the nominal golden.

## Kernel path

Each SysTick calls `xPortSysTickHandler()` → `xTaskIncrementTick()`. While blocked on delay, the task waits on the delayed task list until `xTickCount` reaches its wake time.