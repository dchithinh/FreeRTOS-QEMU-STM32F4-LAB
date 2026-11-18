#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static volatile int idle_seen;

void vApplicationIdleHook(void)
{
    (void)idle_seen;
    /* TODO: observe idle task — set idle_seen when hook runs (enable configUSE_IDLE_HOOK) */
}

static void idle_wait_task(void *arg)
{
    (void)arg;

    while (idle_seen == 0) {
        vTaskDelay(1);
    }
    uart_puts("idle\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(idle_wait_task, "wait", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}