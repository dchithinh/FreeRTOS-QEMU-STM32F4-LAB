# Lab 17 — priority inversion

## Why this lab

A high-priority task can wait a long time on a lock held by a low-priority task while a medium-priority task runs — **priority inversion**. Mutexes with inheritance fix the common case; binary semaphores do not.

## What to memorize

- **Inversion:** `hi` blocked on a resource owned by `low`, while `med` runs at a priority between them.
- **Inheritance:** a mutex temporarily raises the holder to the priority of the highest waiter.
- Binary semaphores used as locks do **not** perform inheritance.

## Your job

Shipped `src/app.c` protects shared work with `xSemaphoreCreateBinary()`. `med` spins; `hi` times out after 3 s and prints `late` (no `DONE`).

Edit `labs/17-priority-inversion/src/app.c` so `hi` prints `DONE` promptly (use a mutex, or otherwise avoid the inversion).

## How to run

```bash
./scripts/lab.sh qemu 17
./scripts/lab.sh solution 17
```

## Done when

`./scripts/lab.sh qemu 17` prints `DONE` and does **not** print `late`.

## GDB

1. `break xQueueSemaphoreTake` / mutex take — see `hi` block and inspect `uxTaskPriorityGet` on `low` before vs after fix.
2. Watch ready lists: with the bug, `med` stays running while `low` stays ready at priority 1.
3. After switching to a mutex, single-step until `low` inherits priority 3, then releases the lock.
4. `info threads` — confirm three tasks and which is Running.