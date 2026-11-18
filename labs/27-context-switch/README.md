# Lab 27 — context switch (PendSV / FPU)

## Why this lab

Cortex-M4F saves integer context in `PendSV_Handler`. When the FPU is in use, the port also **lazily** stacks `s16–s31` (`vstmdbeq` / `vldmiaeq` in `xPortPendSVHandler`).

## What to memorize

- `xPortPendSVHandler` and `vPortSVCHandler` in `third_party/FreeRTOS-Kernel/portable/GCC/ARM_CM4F/port.c`.
- First start: `prvPortStartFirstTask` / `SVC` bootstrap to thread mode with PSP.
- `EXC_RETURN` in `LR` on exception entry: bit 4 (`0x10`) tells the port whether extended FPU frame is active.
- Lazy stacking: `FPCCR` ASPEN/LSPEN set in `xPortStartScheduler`.

## Your job

Run the shipped `src/app.c` (same as solution). `fpu_task` uses `volatile float` so the FPU context is live across switches. `report_task` prints:

1. `fpu`
2. `DONE`

Answer the study questions in the README using GDB and `solution/NOTES.md` (EXC_RETURN / lazy stacking).

## How to run

```bash
./scripts/lab.sh qemu 27
./scripts/lab.sh solution 27
```

## Done when

`fpu` then `DONE` on UART; you can explain `tst r14, #0x10` in the PendSV asm.

## GDB

1. `break xPortPendSVHandler` — disassemble; find `vstmdbeq` guarding FPU save.
2. Let `fpu_task` run, then break on PendSV — check `LR` (`r14`) for `EXC_RETURN` pattern (`0xFFFFFFED` vs `0xFFFFFFE1` class values on M4F).
3. `break vTaskSwitchContext` — correlate with PendSV tail call.
4. Compare PSP before/after a switch for stacked `s16–s31` when FPU is live.

## Questions

1. Which `EXC_RETURN` bit does `tst r14, #0x10` test, and what does a zero result imply?
2. Why is lazy stacking enabled for an MCU RTOS instead of always pushing the full VFP bank?
3. Where is the current task stack pointer stored in the TCB on save (`pxTopOfStack`)?