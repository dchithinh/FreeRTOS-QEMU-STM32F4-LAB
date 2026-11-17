# Lab 02 solution notes

## Why `vTaskDelayUntil`

`vTaskDelay(pdMS_TO_TICKS(50))` after variable work makes each period `50 ms + work time`. `vTaskDelayUntil` advances a fixed wake time by 50 ms per iteration so `a` and `b` stay interleaved.

## Stagger

Task `b` calls `vTaskDelay(pdMS_TO_TICKS(25))` once before its first `vTaskDelayUntil` loop so its first `b` lands 25 ms after the first `a`.

## DONE timing

`done_task` waits 120 ms (past the second `b` at 75 ms) so `DONE` is always last.