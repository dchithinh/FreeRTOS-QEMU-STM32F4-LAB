# Lab 18 — deadlock

## Why this lab

Two tasks that lock the same mutexes in opposite order can each hold one lock and wait forever on the other — **deadlock**.

## What to memorize

- Deadlock needs circular wait (here: `a` holds `m1` wants `m2`, `b` holds `m2` wants `m1`).
- **Lock ordering:** always acquire `m1` then `m2` (or always the same global order).
- `vTaskDelay` between takes gives the other task time to grab its first mutex — makes the hang reliable in QEMU.

## Your job

Shipped `src/app.c` lets `b` take `m2` then `m1` while `a` takes `m1` then `m2`. Symptom: silent hang — no `DONE`.

Edit `labs/18-deadlock/src/app.c` so one task prints `DONE`.

## How to run

```bash
./scripts/lab.sh qemu 18
./scripts/lab.sh solution 18
```

## Done when

`./scripts/lab.sh qemu 18` prints `DONE`.

## GDB

1. `break xQueueTakeMutexRecursive` / mutex take — see both tasks blocked after ~50 ms.
2. Inspect both task states: each should be **Blocked** on a different mutex.
3. `info threads` — neither task reaches the second take.
4. After fix, confirm both tasks use the same lock order and `a` (or `b`) runs through to `DONE`.