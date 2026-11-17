# Lab 06 — notifications

## Why this lab

Each task has a built-in notification word — a lightweight alternative to queues or semaphores when one task must wake another with a small amount of state.

## What to memorize

- `xTaskNotifyGive` / `ulTaskNotifyTake`: fast, per-task; counting-style when take clears on success (`pdTRUE`).
- `eNotifyAction` variants (`SetBits`, `Increment`, `Overwrite`) support different uses: **binary** (zero/one), **counting** (increments), **mailbox** (`Overwrite` with value) — compare to a queue’s depth and structured messages.
- Notifications are not broadcast; you target a specific `TaskHandle_t`.
- Heavier payloads or multiple consumers still belong on a queue.

## Your job

Implement `labs/06-notifications/src/app.c` so UART prints, in order:

1. `n1`
2. `DONE`

Requirements:

- Task **A** calls `xTaskNotifyGive` on task **B**’s handle.
- Task **B** blocks on `ulTaskNotifyTake`, then prints `n1`.
- A third task prints `DONE` after the waiter finishes.
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 06
./scripts/lab.sh check 06
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 06` exits 0 and `./scripts/lab.sh qemu 06` shows the same two lines (modulo QEMU noise before the app runs).