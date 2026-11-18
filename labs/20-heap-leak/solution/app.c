#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "uart.h"

static void reuse_task(void *arg)
{
    uint32_t v;
    uint32_t i;

    (void)arg;
    for (i = 0; i < 64U; i++) {
        QueueHandle_t q = xQueueCreate(4, sizeof(uint32_t));

        if (q == NULL) {
            break;
        }
        v = i;
        xQueueSend(q, &v, 0);
        xQueueReceive(q, &v, 0);
        vQueueDelete(q);
    }
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(reuse_task, "heap", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}