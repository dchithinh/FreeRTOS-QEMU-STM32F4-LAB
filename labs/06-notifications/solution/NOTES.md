# Lab 06 solution notes

## Notification as lightweight signal

`xTaskNotifyGive` increments the notification value; `ulTaskNotifyTake(pdTRUE, …)` waits and clears it (counting mode). No separate queue object — each task has a built-in 32-bit notification word.