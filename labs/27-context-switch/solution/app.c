#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void fpu_task(void *arg)
{
    volatile float f = 1.0f;

    (void)arg;

    for (;;) {
        f += 0.5f;
        if (f > 100.0f) {
            f = 1.0f;
        }
        vTaskDelay(1);
    }
}

static void report_task(void *arg)
{
    (void)arg;

    vTaskDelay(pdMS_TO_TICKS(30));
    uart_puts("fpu\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(fpu_task, "fpu", configMINIMAL_STACK_SIZE * 2, NULL, 3, NULL);
    xTaskCreate(report_task, "rep", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}