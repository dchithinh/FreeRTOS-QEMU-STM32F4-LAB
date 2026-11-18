# Lab 28 solution notes

## Config overlay

`solution/FreeRTOSConfig.h` sets `configUSE_IDLE_HOOK` to `1` on top of `FreeRTOSConfig.default.h`. Learner firmware must do the same and implement `vApplicationIdleHook` in `app.c` (not `bsp/hooks.c`).

## Hook behaviour

The hook only sets `idle_seen = 1`. It must not block: the idle task runs when no other task is ready. `idle_wait_task` spins in `vTaskDelay(1)` until the flag is set, then prints `idle` and `DONE`.

## Pitfall

With `configUSE_IDLE_HOOK` at `0`, the hook symbol is never called and the waiter never observes idle execution, so UART stays silent.