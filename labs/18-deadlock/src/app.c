#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

static SemaphoreHandle_t m1;
static SemaphoreHandle_t m2;

static void task_a(void *arg)
{
    (void)arg;
    xSemaphoreTake(m1, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(50));
    xSemaphoreTake(m2, portMAX_DELAY);
    uart_puts("DONE\r\n");
    xSemaphoreGive(m2);
    xSemaphoreGive(m1);
    vTaskDelete(NULL);
}

static void task_b(void *arg)
{
    (void)arg;
    xSemaphoreTake(m2, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(50));
    xSemaphoreTake(m1, portMAX_DELAY);
    xSemaphoreGive(m1);
    xSemaphoreGive(m2);
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    m1 = xSemaphoreCreateMutex();
    m2 = xSemaphoreCreateMutex();
    configASSERT(m1 != NULL && m2 != NULL);

    xTaskCreate(task_a, "a", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(task_b, "b", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}