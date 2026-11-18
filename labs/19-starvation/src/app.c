#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void spin_task(void *arg)
{
    (void)arg;
    for (;;) {
        volatile uint32_t spin = 0;

        while (spin < 100000U) {
            spin++;
        }
    }
}

static void fair_task(void *arg)
{
    (void)arg;
    uart_puts("fair\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(spin_task, "spin", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(fair_task, "fair", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}