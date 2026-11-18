# Lab 01 solution notes

## Why task notifications?

Without a sync primitive, whether `low` or `high` prints first depends on scheduler start order and time slicing. `high_task` blocks on `ulTaskNotifyTake` until `low_task` calls `xTaskNotifyGive`, so UART order is always `low` then `high`.

## A task function must never return

Unlike a thread on Linux, a FreeRTOS task has no return address. Falling off the end of the function enters `prvTaskExitError()` in `portable/GCC/ARM_CM4F/port.c`: `taskDISABLE_INTERRUPTS()` then `for (;;)`. The scheduler stops for good — in this lab the first `dummy_task` return froze everything after `high`/`low`.

Two valid endings:

```c
for (;;) { vTaskDelay(pdMS_TO_TICKS(100)); }   /* stay alive */
vTaskDelete(NULL);                             /* remove yourself */
```

`main()` is the exception only in the sense that it becomes the idle loop after `vTaskStartScheduler()` fails — but on success it never gets control back either.

## `vTaskDelete` argument

`TaskHandle_t` is already a pointer. Pass the handle value:

```c
vTaskDelete(dummy_h);      /* correct */
vTaskDelete(&dummy_h);     /* TaskHandle_t * — compiler warning, corrupts memory */
vTaskDelete(NULL);         /* means "delete the calling task" */
```

## Stack units

Every `xTaskCreate` stack argument here is `configMINIMAL_STACK_SIZE * 2` (words). Doubling the minimal stack is a common lab default; overflow hooks are enabled in `FreeRTOSConfig.default.h`.

## If `vTaskStartScheduler` returns

That path means idle and/or timer task creation failed. With dynamic allocation, the usual cause is `configTOTAL_HEAP_SIZE` too small for kernel housekeeping plus your tasks. The malloc failed hook would print `HOOK MALLOC_FAILED`.

## Suspend/resume path

`paused` is created, immediately suspended in `main`, then resumed from `high_task` after `high` is printed so `resumed` and `DONE` appear last.