#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void heap_task(void *arg)
{
    void *block;

    (void)arg;
    block = pvPortMalloc(16);
    if (block != NULL) {
        uart_puts("alloc_ok\r\n");
        vPortFree(block);
    }
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(heap_task, "heap", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}