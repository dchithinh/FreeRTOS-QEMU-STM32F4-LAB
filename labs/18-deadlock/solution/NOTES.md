# Lab 18 solution notes

`a` takes `m1` then `m2`; `b` takes `m2` then `m1`. After each task holds its first mutex and delays, both block forever on the second — classic deadlock.

Fix: acquire **m1 before m2** in both tasks (total lock ordering).