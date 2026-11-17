# Lab 01 solution notes

## Why task notifications?

Without a sync primitive, whether `low` or `high` prints first depends on scheduler start order and time slicing. `high_task` blocks on `ulTaskNotifyTake` until `low_task` calls `xTaskNotifyGive`, so UART order is always `low` then `high`.

## Stack units

Every `xTaskCreate` stack argument here is `configMINIMAL_STACK_SIZE * 2` (words). Doubling the minimal stack is a common lab default; overflow hooks are enabled in `FreeRTOSConfig.default.h`.

## If `vTaskStartScheduler` returns

That path means idle and/or timer task creation failed. With dynamic allocation, the usual cause is `configTOTAL_HEAP_SIZE` too small for kernel housekeeping plus your tasks. The malloc failed hook would print `HOOK MALLOC_FAILED`.

## Suspend/resume path

`paused` is created, immediately suspended in `main`, then resumed from `high_task` after `high` is printed so `resumed` and `DONE` appear last.