#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

static SemaphoreHandle_t mtx;

static void first_locker(void *arg)
{
    (void)arg;
    xSemaphoreTake(mtx, portMAX_DELAY);
    uart_puts("locked\r\n");
    xSemaphoreGive(mtx);
    vTaskDelete(NULL);
}

static void second_locker(void *arg)
{
    (void)arg;
    xSemaphoreTake(mtx, portMAX_DELAY);
    uart_puts("unlocked\r\n");
    xSemaphoreGive(mtx);
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

    mtx = xSemaphoreCreateMutex();
    xTaskCreate(first_locker, "l1", configMINIMAL_STACK_SIZE * 2, NULL, 3, NULL);
    xTaskCreate(second_locker, "l2", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(done_task, "d", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}