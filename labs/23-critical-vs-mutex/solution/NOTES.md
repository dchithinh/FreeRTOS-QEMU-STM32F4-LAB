# Lab 23 solution notes

All three mechanisms serialize access to `counter` for these short, task-only increments:

1. **Critical section** — masks interrupts up to `configMAX_SYSCALL_INTERRUPT_PRIORITY`; ISR-safe when used correctly; keep duration tiny.
2. **Scheduler lock** — `vTaskSuspendAll` stops context switches but does not stop ISRs; not ISR-safe; no blocking APIs while suspended.
3. **Mutex** — task-level lock; can block; not for ISR use; supports priority inheritance on FreeRTOS mutexes.