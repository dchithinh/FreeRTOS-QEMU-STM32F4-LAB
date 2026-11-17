# Lab 03 solution notes

## Queue sizing

`xQueueCreate(1, sizeof(uint32_t))` holds one 32-bit copy. A second send without a receive would block (or fail with zero block time).

## Copy semantics

`xQueueSend` copies `val` into queue storage; the producer’s stack variable can go out of scope after a successful send.

## DONE line

The consumer prints the received value then `DONE`, so no extra sync primitive is required.