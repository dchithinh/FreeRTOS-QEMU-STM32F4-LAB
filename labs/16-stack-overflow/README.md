# Lab 16 — stack overflow hook

## Why this lab

Stack overruns corrupt TCBs and crash later in unrelated code. `configCHECK_FOR_STACK_OVERFLOW` plus `vApplicationStackOverflowHook` turns that into a loud, GDB-friendly failure.

## What to memorize

- Stack sizes in `xTaskCreate` are in **words**, not bytes.
- `configCHECK_FOR_STACK_OVERFLOW 2` — canary markers checked on context switch.
- This BSP’s hook prints `HOOK STACK_OVERFLOW` then hangs (see `bsp/hooks.c`).

## Your job

Shipped `src/app.c` allocates a multi-kB buffer on a minimal stack. Symptom:

1. `HOOK STACK_OVERFLOW`
2. hang (no `DONE`)

Edit `labs/16-stack-overflow/src/app.c` so UART prints `DONE` without tripping the hook.

## How to run

```bash
./scripts/lab.sh qemu 16
./scripts/lab.sh solution 16
```

Broken firmware is useful to study once: you should see the hook line on learner `qemu 16` before you fix it.

## Done when

`./scripts/lab.sh qemu 16` prints `DONE` and does **not** print `HOOK STACK_OVERFLOW`.

## GDB

1. `break vApplicationStackOverflowHook` — confirm the task name argument when the bug is present.
2. `break vTaskSwitchContext` / step stack-check path when `configCHECK_FOR_STACK_OVERFLOW 2` is enabled.
3. `xTaskGetStackHighWaterMark` from a healthy task — compare headroom before and after increasing stack size.
4. Inspect the task’s stack base in the TCB when the canary is corrupted.