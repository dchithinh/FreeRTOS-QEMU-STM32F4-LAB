#include "FreeRTOS.h"
#include "irq.h"
#include "queue.h"
#include "task.h"
#include "uart.h"

static QueueHandle_t irq_q;

void BSP_IRQHandler(void)
{
    uint32_t m = 1;

    /* Bug: task-level API inside an ISR — undefined / assert on CM port. */
    xQueueSend(irq_q, &m, 0);
}

static void receiver_task(void *arg)
{
    uint32_t m;

    (void)arg;
    xQueueReceive(irq_q, &m, portMAX_DELAY);
    (void)m;
    uart_puts("isr\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

static void pender_task(void *arg)
{
    (void)arg;
    for (;;) {
        bsp_irq_pend();
    }
}

int main(void)
{
    uart_init();
    bsp_irq_init(5);

    irq_q = xQueueCreate(4, sizeof(uint32_t));
    xTaskCreate(receiver_task, "rx", configMINIMAL_STACK_SIZE * 2, NULL, 3, NULL);
    xTaskCreate(pender_task, "pend", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}