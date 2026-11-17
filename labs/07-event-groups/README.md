# Lab 07 — event groups

## Why this lab

Event groups hold a bitmask of events. Tasks can wait until **any** bit is set (wait-any) or **all** of a mask is set (wait-all) — useful when several conditions must line up before continuing.

## What to memorize

- `xEventGroupCreate`, `xEventGroupSetBits`, `xEventGroupWaitBits`: wait with `pdTRUE` for clear-on-exit, `pdFALSE` to leave bits latched.
- **Wait-all** vs **wait-any**: pass a bit mask and set `xWaitForAllBits` to `pdTRUE` (all) or `pdFALSE` (any).
- `xEventGroupSync`: set one bit, then block until a **combined** mask is set (rendezvous / barrier).
- Event groups are heavier than task notifications; use when multiple boolean conditions are combined.

## Your job

Implement `labs/07-event-groups/src/app.c` so UART prints, in order:

1. `sync`
2. `DONE`

Requirements:

- Two tasks each call `xEventGroupSync` with **bit 1** and **bit 2** (`BIT_ONE`, `BIT_TWO`) waiting for both.
- After the barrier, **one** task prints `sync`.
- A third task prints `DONE` after the sync pair finish.
- Call `uart_init()` before `vTaskStartScheduler()`.

Compare your output with `expected.log`. Peek at `solution/app.c` only after you try.

## How to run

From the repo root:

```bash
./scripts/lab.sh qemu 07
./scripts/lab.sh check 07
```

`check` builds the **solution** ELF and diffs UART output against `expected.log` (your `src/` is what you exercise in `qemu`).

## Done when

`./scripts/lab.sh check 07` exits 0 and `./scripts/lab.sh qemu 07` shows the same two lines (modulo QEMU noise before the app runs).