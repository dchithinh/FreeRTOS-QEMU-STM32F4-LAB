#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "uart.h"

static QueueHandle_t data_q;

static void producer(void *arg)
{
    (void)arg;
    /* TODO: xQueueSend uint32_t value 42 */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void consumer(void *arg)
{
    (void)arg;
    /* TODO: xQueueReceive and print the value, then DONE */
    for (;;) {
        vTaskDelay(1000);
    }
}

int main(void)
{
    uart_init();

    /* TODO: xQueueCreate length 1, item sizeof(uint32_t) */
    /* TODO: xTaskCreate producer and consumer */
    (void)data_q;
    vTaskStartScheduler();
    for (;;) {
    }
}