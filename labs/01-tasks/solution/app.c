#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static TaskHandle_t high_h;
static TaskHandle_t paused_h;

static void paused(void *arg)
{
    (void)arg;
    uart_puts("resumed\r\nDONE\r\n");
    for (;;) {
        vTaskDelay(1000);
    }
}

static void high_task(void *arg)
{
    (void)arg;
    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    uart_puts("high\r\n");
    vTaskResume(paused_h);
    for (;;) {
        vTaskDelay(1000);
    }
}

static void low_task(void *arg)
{
    (void)arg;
    uart_puts("low\r\n");
    xTaskNotifyGive(high_h);
    for (;;) {
        vTaskDelay(1000);
    }
}

static void dummy_task(void *arg)
{
    (void)arg;
}

int main(void)
{
    uart_init();

    {
        TaskHandle_t tmp;
        xTaskCreate(dummy_task, "tmp", configMINIMAL_STACK_SIZE * 2, NULL, 1, &tmp);
        vTaskDelete(tmp);
    }

    xTaskCreate(paused, "p", configMINIMAL_STACK_SIZE * 2, NULL, 1, &paused_h);
    vTaskSuspend(paused_h);
    xTaskCreate(high_task, "h", configMINIMAL_STACK_SIZE * 2, NULL, 3, &high_h);
    xTaskCreate(low_task, "l", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}