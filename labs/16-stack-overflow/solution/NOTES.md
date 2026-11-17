# Lab 16 solution notes

With `configCHECK_FOR_STACK_OVERFLOW 2`, the kernel checks canary words on context switch. The broken `src` uses deep recursion with 64-byte locals on a `configMINIMAL_STACK_SIZE` stack, then calls the same `vApplicationStackOverflowHook` in `bsp/hooks.c` (QEMU may hard-fault before the kernel prints on a single huge frame).

Fix: give the task enough stack words for peak usage (or flatten recursion / shrink locals). The reference solution uses a 4× minimal stack and a tiny worker that prints `DONE`.