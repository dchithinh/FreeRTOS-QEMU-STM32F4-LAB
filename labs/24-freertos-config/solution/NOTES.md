# Lab 24 solution notes

## Fix

Learner `FreeRTOSConfig.h` sets `configTICK_RATE_HZ` to `100`, so ten `vTaskDelay(pdMS_TO_TICKS(10))` calls advance only ~10 ticks while wall time is still ~100 ms. The solution overlay restores `configTICK_RATE_HZ` to `1000` (via `#undef` / `#define` after including `FreeRTOSConfig.default.h`).

## Pitfall

`pdMS_TO_TICKS` uses the **current** `configTICK_RATE_HZ`. A wrong tick rate makes delays and the printed `ticks=` disagree even when the delay loop “feels” correct in milliseconds.

## SysTick

`configCPU_CLOCK_HZ` in `FreeRTOSConfig.default.h` is `168000000` to match QEMU `netduinoplus2` and the BSP UART clock comment. `vPortSetupTimerInterrupt()` derives the SysTick reload from that value and `configTICK_RATE_HZ`.