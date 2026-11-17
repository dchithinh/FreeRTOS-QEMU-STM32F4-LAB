#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void task_a(void *arg)
{
    (void)arg;
    TickType_t wake = xTaskGetTickCount();
    for (int i = 0; i < 2; i++) {
        uart_puts("a\r\n");
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}

static void task_b(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(25));
    TickType_t wake = xTaskGetTickCount();
    for (int i = 0; i < 2; i++) {
        uart_puts("b\r\n");
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}

static void done_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(120));
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(task_a, "a", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(task_b, "b", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(done_task, "d", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}