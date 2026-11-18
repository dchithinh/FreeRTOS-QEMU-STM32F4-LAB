#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

static SemaphoreHandle_t lock;

static void low_task(void *arg)
{
    (void)arg;
    xSemaphoreTake(lock, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(200));
    xSemaphoreGive(lock);
    vTaskDelete(NULL);
}

static void high_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(20));
    xSemaphoreTake(lock, portMAX_DELAY);
    uart_puts("DONE\r\n");
    xSemaphoreGive(lock);
    vTaskDelete(NULL);
}

static void medium_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(40));
    for (;;) {
        volatile uint32_t spin = 0;

        while (spin < 200000U) {
            spin++;
        }
    }
}

int main(void)
{
    uart_init();

    lock = xSemaphoreCreateMutex();
    configASSERT(lock != NULL);

    xTaskCreate(low_task, "low", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    xTaskCreate(medium_task, "med", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(high_task, "hi", configMINIMAL_STACK_SIZE * 2, NULL, 3, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}