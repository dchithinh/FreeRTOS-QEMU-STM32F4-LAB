# Lab 21 — timer callback blocks

## Why this lab

Software timer callbacks run in the **timer daemon task**, not in an ISR. Blocking APIs (`vTaskDelay`, mutex waits, long loops) stall **all** timer processing.

## What to memorize

- One blocked timer callback delays every other timer and deferred function call behind it.
- Keep callbacks short; defer work to a task (`xTaskNotifyGive`, queues).
- `xTimerStart(..., portMAX_DELAY)` blocks the caller until the start command reaches the daemon — that is fine in a task.

## Your job

Shipped `src/app.c` uses `vTaskDelay(100)` inside `slow_cb`. `fast_tmr` should bump `fast_hits`, but the daemon never services it. `worker_task` waits forever — no `DONE`.

Edit `labs/21-timer-callback-blocks/src/app.c` so the slow callback only notifies the worker and `fast_hits` reaches 4; worker prints `DONE`.

## How to run

```bash
./scripts/lab.sh qemu 21
./scripts/lab.sh solution 21
```

## Done when

`./scripts/lab.sh qemu 21` prints `DONE` before QEMU times out.

## GDB

1. `break slow_cb` — single-step into `vTaskDelay`; watch the timer daemon stack.
2. `break xTimerGenericCommand` — see the command queue stop draining while the daemon is blocked.
3. After the fix, break `fast_cb` and confirm `fast_hits` increments while `slow_cb` stays non-blocking.
4. `info threads` — locate the `Tmr Svc` thread and its priority (2 by default).