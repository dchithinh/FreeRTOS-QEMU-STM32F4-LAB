# Lab 09 solution notes

## Single writer, single reader

Stream buffers are documented for **exactly one task writing and one task reading**. Two writers can interleave bytes unpredictably or corrupt trigger/wait state; two readers can steal each other's data. Use a mutex or a queue if multiple producers or consumers must share the path.