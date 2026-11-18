# Lab 01 — tasks

## Why this lab

FreeRTOS is task-centric: you split work into concurrent tasks and let the scheduler decide what runs. Before queues or mutexes, you need `xTaskCreate`, priorities, delays, and suspend/resume so you can reason about *who runs when*.

## What to memorize

- `xTaskCreate`: stack size is in **words**, not bytes (`256` = 256 × `sizeof(StackType_t)`).
- Priorities: higher number = more urgent; idle is priority 0; config allows `configMAX_PRIORITIES` levels.
- Task states: ready, running, blocked (delay, wait), suspended.
- `vTaskDelay(ticks)` blocks the current task for at least that many tick periods.
- `vTaskSuspend` / `vTaskResume` remove a task from scheduling without deleting it.
- `vTaskDelete` frees a task’s kernel objects; call only when the task must not run again.
- `vTaskStartScheduler()` never returns on success; if it does, the idle/timer tasks could not be created — usually **heap too small** (`configTOTAL_HEAP_SIZE`).
- **A task function must never return.** There is no caller to return to. Returning lands in `prvTaskExitError()` (port `port.c`): interrupts stay off and the CPU spins forever, so every other task stops. End every task with an infinite loop, or call `vTaskDelete(NULL)` last.
- `vTaskDelete` takes the handle itself: `vTaskDelete(dummy_h)`, **not** `vTaskDelete(&dummy_h)`.

This lab uses task notifications only to order UART lines deterministically; notifications are not on the memorize list for the exam-style takeaway.

## Your job

Implement `labs/01-tasks/src/app.c` so UART prints, in order:

1. `low`
2. `high`
3. `resumed`
4. `DONE`

Requirements:

- Create a **paused** task, then `vTaskSuspend` it before the scheduler starts; later `vTaskResume` it so it prints `resumed`.
- Use at least two runnable priorities besides the paused task (high and low).
- Use `vTaskDelete` on a throwaway task in `main` before `vTaskStartScheduler` (see solution pattern).
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 01
./scripts/lab.sh check 01
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 01` exits 0 and `./scripts/lab.sh qemu 01` shows the same four lines (modulo QEMU noise before the app runs).