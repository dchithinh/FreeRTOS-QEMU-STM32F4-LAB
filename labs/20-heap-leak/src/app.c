#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "uart.h"

static void leak_task(void *arg)
{
    (void)arg;

    for (;;) {
        QueueHandle_t q = xQueueCreate(4, sizeof(uint32_t));

        if (q == NULL) {
            break;
        }
    }
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(leak_task, "leak", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}