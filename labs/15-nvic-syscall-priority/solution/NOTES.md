# Lab 15 solution notes

On Cortex-M, **lower NVIC numbers = higher logical priority**. Only interrupts at or below `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (5 on this BSP) may call FreeRTOS FromISR APIs.

`bsp_irq_init(0)` makes EXTI0 too high; the port raises `configMAX_SYSCALL_INTERRUPT_PRIORITY` / BASEPRI checks and `xQueueSendFromISR` fails its assert.

Match `bsp_irq_init(5)` to `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` in `FreeRTOSConfig.default.h`.