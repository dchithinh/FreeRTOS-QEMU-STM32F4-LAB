#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "uart.h"

static QueueHandle_t msg_q;

static void worker_task(void *arg)
{
    uint32_t msg = 1;

    (void)arg;
    xQueueSend(msg_q, &msg, portMAX_DELAY);
    xQueueReceive(msg_q, &msg, portMAX_DELAY);
    (void)msg;
    uart_puts("alive\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
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