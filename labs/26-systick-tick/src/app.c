#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void tick_task(void *arg)
{
    (void)arg;

    /* TODO: vTaskDelay(5), then print tick count and DONE (see README) */
    for (;;) {
        vTaskDelay(1000);
    }
}

int main(void)
{
    uart_init();

    xTaskCreate(tick_task, "tick", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}