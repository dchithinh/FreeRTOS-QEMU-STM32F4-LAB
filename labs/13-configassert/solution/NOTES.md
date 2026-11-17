# Lab 13 solution notes

`xQueueReceive` requires a valid `QueueHandle_t`. Passing `NULL` trips `configASSERT` in `queue.c` before any blocking occurs — the firmware spins in the assert loop with interrupts off, so UART never reaches `DONE`.

Fix: pass `msg_q` (or any created queue) and use the API only after `xQueueCreate` succeeds.