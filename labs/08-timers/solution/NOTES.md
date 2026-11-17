# Lab 08 solution notes

## Timer service task

Software timer callbacks run in the **Timer Service** (daemon) task, not in the task that called `xTimerStart`. Keep callbacks short — no blocking APIs; `xTimerStop(..., 0)` from the callback is allowed (queue to daemon with zero block time).