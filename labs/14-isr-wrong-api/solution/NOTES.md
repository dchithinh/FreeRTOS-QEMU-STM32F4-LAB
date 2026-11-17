# Lab 14 solution notes

`xQueueSend` uses a critical section and scheduler state that is only legal in task context. Inside `BSP_IRQHandler` the CM4F port detects ISR context and asserts (or misbehaves).

Use `xQueueSendFromISR`, pass `BaseType_t *pxHigherPriorityTaskWoken`, and end with `portYIELD_FROM_ISR(woken)` so the blocked receiver can run promptly.