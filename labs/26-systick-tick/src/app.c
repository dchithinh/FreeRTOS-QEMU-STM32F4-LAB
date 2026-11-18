#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void tick_task(void *arg)
{
    (void)arg;

    /* TODO: xTaskGetTickCount before/after vTaskDelay(5); print tick <elapsed> and DONE */
    for (;;) {
        vTaskDelay(5);
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