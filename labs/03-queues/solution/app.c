#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "uart.h"

static QueueHandle_t data_q;

static void producer(void *arg)
{
    (void)arg;
    uint32_t val = 42;
    xQueueSend(data_q, &val, portMAX_DELAY);
    vTaskDelete(NULL);
}

static void consumer(void *arg)
{
    (void)arg;
    uint32_t val;
    xQueueReceive(data_q, &val, portMAX_DELAY);
    (void)val;
    uart_puts("42\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    data_q = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(producer, "prod", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(consumer, "cons", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}