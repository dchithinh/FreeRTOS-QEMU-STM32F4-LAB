# Lab 16 solution notes

With `configCHECK_FOR_STACK_OVERFLOW 2`, the kernel checks canary words on context switch. The broken `src` uses deep recursion with 80-byte locals on a `configMINIMAL_STACK_SIZE` stack until method 2 trips `vApplicationStackOverflowHook` in `bsp/hooks.c` (a single multi-kB stack frame can hard-fault on QEMU before the canary runs).

Fix: give the task enough stack words for peak usage (or flatten recursion / shrink locals). The reference solution uses a 4× minimal stack and a tiny worker that prints `DONE`.