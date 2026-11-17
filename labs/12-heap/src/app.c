#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void heap_task(void *arg)
{
    (void)arg;
    /* TODO: pvPortMalloc 16 bytes; on success print alloc_ok; vPortFree; print DONE */
}

int main(void)
{
    uart_init();

    /* TODO: create heap_task before vTaskStartScheduler */
    vTaskStartScheduler();
    for (;;) {
    }
}