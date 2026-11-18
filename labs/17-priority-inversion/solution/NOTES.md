# Lab 17 solution notes

`low` holds a **binary semaphore** while `med` (priority 2) spins. `hi` (priority 3) blocks on the sem; without **priority inheritance** `low` never runs, so `hi` hits the 3 s take timeout and prints `late` — not `DONE`.

Fix: use `xSemaphoreCreateMutex()` for the lock. Inheritance temporarily boosts `low` above `med` until the mutex is released, then `hi` prints `DONE`.