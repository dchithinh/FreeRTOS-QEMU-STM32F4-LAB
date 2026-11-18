# Lab 20 solution notes

Each `xQueueCreate` allocates from `heap_4`. Shipped `src` keeps every handle and never calls `vQueueDelete`, so the 8 KiB heap exhausts and `vApplicationMallocFailedHook` prints `HOOK MALLOC_FAILED`.

Fix: delete each queue after use, or keep one queue and reuse it. Then the task can print `DONE`.