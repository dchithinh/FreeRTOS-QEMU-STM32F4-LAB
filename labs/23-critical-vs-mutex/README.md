# Lab 23 — critical section vs suspend-all vs mutex

## Why this lab

FreeRTOS gives three different ways to serialize task access to a shared variable. They differ in ISR safety, whether the caller can block, and how long you should hold them.

## What to memorize

| Mechanism | ISR-safe? | Can block? | Typical duration |
|-----------|-----------|------------|------------------|
| `taskENTER_CRITICAL` / `taskEXIT_CRITICAL` | Yes (task context) | No | Very short — a few instructions |
| `vTaskSuspendAll` / `xTaskResumeAll` | No | No | Short — no API that may block |
| Mutex (`xSemaphoreTake`) | No | Yes (waits for holder) | As long as a normal critical region |

`taskENTER_CRITICAL` only protects against interrupt preemption up to the syscall mask; it does **not** stop other tasks unless they also hit a kernel call that checks the mask.

## Your job

Shipped `src/app.c` runs three back-to-back demos (critical, suspend-all, mutex) but leaves every `TODO` empty — each test under-counts and UART prints `crit=… susp=… mtx=…` without `ok` or `DONE`.

Fill in the three `TODO` blocks so each test reaches `2000`, then the runner prints `ok` and `DONE`.

## How to run

```bash
./scripts/lab.sh qemu 23
./scripts/lab.sh solution 23
```

## Done when

`./scripts/lab.sh qemu 23` prints `ok` then `DONE`.

## GDB

1. `break inc_task` with `mode == PROT_CRITICAL` — step through `taskENTER_CRITICAL` and watch `basepri` on CM4.
2. Repeat for `vTaskSuspendAll` — note other tasks stay ready but do not run until `xTaskResumeAll`.
3. On the mutex path, `break xQueueSemaphoreTake` — see the second task block when both hit the counter hot.
4. Compare wall time per mechanism — critical/suspend should finish faster than mutex-heavy designs with contention.