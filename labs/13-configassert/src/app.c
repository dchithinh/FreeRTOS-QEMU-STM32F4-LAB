#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "uart.h"

static QueueHandle_t msg_q;

static void worker_task(void *arg)
{
    uint32_t msg;

    (void)arg;
    /* Bug: NULL queue handle — configASSERT fires inside xQueueReceive. */
    xQueueReceive(NULL, &msg, portMAX_DELAY);
}

int main(void)
{
    uart_init();

    msg_q = xQueueCreate(4, sizeof(uint32_t));
    xTaskCreate(worker_task, "work", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}