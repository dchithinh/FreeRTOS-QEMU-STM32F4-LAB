# Lab 21 solution notes

Timer callbacks run in the **Timer Service** task (the daemon). `vTaskDelay` in `slow_cb` blocks that task, so `fast_tmr` never gets serviced and `fast_hits` stays at zero.

Fix: keep the callback minimal — `xTaskNotifyGive(worker_h)` — and do any blocking work in a normal task.