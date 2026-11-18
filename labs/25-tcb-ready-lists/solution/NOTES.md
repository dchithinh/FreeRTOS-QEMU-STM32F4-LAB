# Lab 25 solution notes

## Ordering

`p3`, `p2`, and `p1` run at priorities 3, 2, and 1 and print `3`, `2`, and `1`. The preemptive scheduler always runs the highest ready priority first, so UART order follows priority even though the tasks were created in mixed order.

## Waiter at priority 1

`waiter_task` shares priority 1 with `p1` but starts first and blocks in `vTaskDelay` until `done_mask == 0x0EU`. The three `pri_task` instances each set one bit in `done_mask` before deleting themselves; only then does the waiter print `DONE`.

## GDB tie-in

When breaking on `vTaskSwitchContext`, compare `((TCB_t *)pxCurrentTCB)->uxPriority` to the line you see on UART. Ready-list heads are `pxReadyTasksLists[priority]` in `tasks.c`.