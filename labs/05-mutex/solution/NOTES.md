# Lab 05 solution notes

## Serialize with one mutex

`first_locker` runs at priority 3 and acquires the mutex before `second_locker` (priority 2) can run past its block on `xSemaphoreTake`. Prints stay `locked` then `unlocked`.

## Mutex vs binary semaphore

A mutex records **ownership** (only the holder should give). A binary semaphore signals events and has no owner rule — do not use it to protect shared mutable data when arbitrary tasks may take/give.