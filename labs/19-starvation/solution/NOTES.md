# Lab 19 solution notes

Shipped firmware runs `spin` at priority 3 and `fair` at 2; `spin` never blocks, so `fair` never runs. `FreeRTOSConfig.h` also documents `configUSE_TIME_SLICING 0` (equal-priority starvation); the shared kernel build may still time-slice same priorities — the priority gap is what makes the bug reliable in QEMU.

Fix options: raise `fair` above `spin`, add `vTaskDelay` in `spin`, or enable time-slicing / lower the hog task’s priority.