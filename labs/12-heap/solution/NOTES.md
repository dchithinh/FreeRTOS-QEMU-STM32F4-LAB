# Lab 12 solution notes

## Heap at scheduler start

Even before your task runs, starting the scheduler allocates TCB/stack for the idle task (and the timer service when `configUSE_TIMERS` is 1). Size `configTOTAL_HEAP_SIZE` with that overhead in mind; this lab's small malloc should still succeed on the default 48 KiB heap.