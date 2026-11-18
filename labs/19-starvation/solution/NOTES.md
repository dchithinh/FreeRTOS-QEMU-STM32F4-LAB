# Lab 19 solution notes

Shipped `src` runs `spin` and `fair` at priority 2 with `configUSE_TIME_SLICING 0` in the lab `FreeRTOSConfig.h`; the kernel is built with that overlay, so `fair` never runs.

The solution enables `configUSE_TIME_SLICING 1` in `solution/FreeRTOSConfig.h` (kernel and app use that header). Equal-priority tasks round-robin on the tick; `fair` prints then `DONE`.

Alternates: add `vTaskDelay` in `spin`, or raise `fair` above `spin`.