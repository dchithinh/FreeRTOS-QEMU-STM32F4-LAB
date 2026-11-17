#include "FreeRTOS.h"
#include "irq.h"
#include "queue.h"
#include "task.h"
#include "uart.h"

static QueueHandle_t irq_q;

void BSP_IRQHandler(void)
{
    /* TODO: use xQueueSendFromISR (not xQueueSend); portYIELD_FROM_ISR if woken */
}

static void receiver_task(void *arg)
{
    (void)arg;
    /* TODO: block on queue; print isr; print DONE; delete */
}

static void pender_task(void *arg)
{
    (void)arg;
    /* TODO: loop bsp_irq_pend() */
}

int main(void)
{
    uart_init();
    bsp_irq_init(5);

    /* TODO: create queue; receiver and pender tasks */
    (void)irq_q;
    vTaskStartScheduler();
    for (;;) {
    }
}