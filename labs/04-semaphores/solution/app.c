#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

static SemaphoreHandle_t count_sem;

static void taker(void *arg)
{
    (void)arg;
    xSemaphoreTake(count_sem, portMAX_DELAY);
    uart_puts("took\r\n");
    vTaskDelete(NULL);
}

static void done_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(100));
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    count_sem = xSemaphoreCreateCounting(2, 0);
    xSemaphoreGive(count_sem);
    xSemaphoreGive(count_sem);
    xTaskCreate(taker, "t1", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(taker, "t2", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(done_task, "d", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}