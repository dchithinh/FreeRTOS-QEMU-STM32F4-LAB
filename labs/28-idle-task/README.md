# Lab 28 — idle task and idle hook

## Why this lab

When no application task is ready, the **idle task** (priority 0) runs. With `configUSE_IDLE_HOOK`, the kernel calls `vApplicationIdleHook()` on each idle iteration — useful for minimal background work (never block inside the hook).

## What to memorize

- Idle task created inside `vTaskStartScheduler()` in `third_party/FreeRTOS-Kernel/tasks.c`.
- `configUSE_IDLE_HOOK` must be `1` in the lab’s `FreeRTOSConfig.h` and you must provide `vApplicationIdleHook` in application code (this repo uses `app.c`, not `bsp/hooks.c`).
- Tickless idle is **not** enabled in this BSP (`configUSE_TICKLESS_IDLE` stays 0); the idle task simply yields or runs the hook.

## Your job

Shipped `src/app.c` leaves `vApplicationIdleHook` empty and leaves `configUSE_IDLE_HOOK` at `0`. UART never prints `idle` or `DONE`.

Fix `FreeRTOSConfig.h` (set `configUSE_IDLE_HOOK` to `1`) and implement the hook so a waiting task sees idle execution and prints:

1. `idle`
2. `DONE`

The solution overlay lives in `solution/FreeRTOSConfig.h` for reference.

## How to run

```bash
./scripts/lab.sh qemu 28
./scripts/lab.sh solution 28
```

## Done when

`idle` then `DONE` on learner firmware.

## GDB

1. `break vApplicationIdleHook` — confirm it fires only when the ready lists are empty except idle.
2. `break prvIdleTask` in `tasks.c` — step into the hook call path.
3. Watch `idle_wait_task`: it blocks on `vTaskDelay` while no other application task is ready; hook sets your flag.
4. `info tasks` when stalled — idle should run at priority 0.