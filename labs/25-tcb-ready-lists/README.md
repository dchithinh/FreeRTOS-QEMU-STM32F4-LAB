# Lab 25 — TCB and ready lists

## Why this lab

The scheduler picks work from **ready lists**, one per priority. Each task’s control block (`TCB_t` in `third_party/FreeRTOS-Kernel/tasks.c`) links into `pxReadyTasksLists[priority]` while the task is ready to run.

## What to memorize

- `typedef struct tskTaskControlBlock` (~line 358 in `tasks.c`): stack pointer, priority, state list item.
- `pxReadyTasksLists[ configMAX_PRIORITIES ]` — array of `List_t` heads (~line 459).
- Preemptive pick: highest non-empty ready list wins; within a priority, round-robin via `listGET_OWNER_OF_NEXT_ENTRY`.
- `vTaskStartScheduler()` creates idle and (if enabled) timer tasks before the first context switch.

## Your job

Run the shipped firmware (`src/app.c` matches solution). UART must print, in order:

1. `3`
2. `2`
3. `1`
4. `DONE`

Three tasks at priorities 3, 2, and 1 each print their priority once; a waiter at priority 1 prints `DONE` after all three finish.

Use GDB and the README kernel paths to map each running task back to a TCB and ready list.

## How to run

```bash
./scripts/lab.sh qemu 25
./scripts/lab.sh solution 25
```

## Done when

Output order is `3`, `2`, `1`, `DONE`.

## GDB

1. `break vTaskStartScheduler` — step until idle task exists; scheduler is about to run first task.
2. After start, `break vTaskSwitchContext` — note which TCB’s `uxPriority` matches the running task.
3. `print pxReadyTasksLists[3]` / `[2]` / `[1]` — watch list lengths as tasks run and delete.
4. `p/x *(TCB_t *)pxCurrentTCB` fields — tie `uxPriority` to UART order.