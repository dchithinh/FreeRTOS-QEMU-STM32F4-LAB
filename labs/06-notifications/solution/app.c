#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static TaskHandle_t waiter_h;

static void notifier(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(10));
    xTaskNotifyGive(waiter_h);
    vTaskDelete(NULL);
}

static void waiter(void *arg)
{
    (void)arg;
    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    uart_puts("n1\r\n");
    vTaskDelete(NULL);
}

static void done_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(50));
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(waiter, "wait", configMINIMAL_STACK_SIZE * 2, NULL, 2, &waiter_h);
    xTaskCreate(notifier, "note", configMINIMAL_STACK_SIZE * 2, NULL, 3, NULL);
    xTaskCreate(done_task, "d", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}