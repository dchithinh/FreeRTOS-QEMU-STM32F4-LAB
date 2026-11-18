# Lab 27 — answers (EXC_RETURN / lazy stacking)

## `tst r14, #0x10`

On exception entry, `LR` holds `EXC_RETURN`. Bit 4 (`0x10`) is **0** when the floating-point context is active (extended stack frame). The PendSV handler tests that bit: if zero (`eq`), it pushes/pops `s16–s31` with `vstmdbeq` / `vldmiaeq`. If bit 4 is 1, only the core integer frame is saved.

Typical values you may see in GDB include `0xFFFFFFED` (return to Thread mode, PSP, FPU frame) versus `0xFFFFFFE1` (no FPU state). Exact values depend on whether the FPU was used before the exception.

## Lazy stacking

The CM4F port sets `FPCCR` ASPEN and LSPEN in `xPortStartScheduler`. Hardware can defer pushing the VFP bank until the FPU is actually touched, which keeps interrupt latency lower for ISRs that do not use the FPU. FreeRTOS PendSV still performs the explicit `s16–s31` save/restore when `EXC_RETURN` indicates an extended frame—so a task using `float` forces the full lazy path on context switch.

## TCB stack pointer

In `tasks.c`, the first member of `TCB_t` is the task’s top-of-stack pointer. PendSV stores the decremented PSP there after pushing `r4–r11`, `r14`, and optionally `s16–s31`.