# Lab 22 — data race

## Why this lab

Two tasks updating one variable without synchronization lose updates. The final value is unpredictable — almost always less than the sum you expect.

## What to memorize

- A C `counter++` is not atomic on ARM; another task can run between load and store.
- Protect shared mutable state with a mutex, critical section, or disable scheduling — pick the tool for the context.
- Do not print inside the hot loop here; UART output already masks other bugs with critical sections.

## Your job

Shipped `src/app.c` runs two tasks that each increment a shared `counter` 10,000 times with **no** lock. The reporter prints the count (always **below** `20000`) and never prints `DONE`.

Edit `labs/22-data-race/src/app.c` so the printed count is exactly `20000` followed by `DONE`.

## How to run

```bash
./scripts/lab.sh qemu 22
./scripts/lab.sh solution 22
```

## Done when

`./scripts/lab.sh qemu 22` prints `20000` then `DONE`.

## GDB

1. `break inc_task` and watch `counter` — single-step one increment; note when the other task interleaves.
2. Set a data watchpoint on `counter` if your gdb build supports it on QEMU.
3. After adding a mutex, confirm both tasks complete and `counter == 20000` before the reporter runs.
4. `info threads` while both incrementers run — both should be `Ready`/`Running` at priority 2.