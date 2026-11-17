#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void worker_task(void *arg)
{
    (void)arg;
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(worker_task, "work", configMINIMAL_STACK_SIZE * 4, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}