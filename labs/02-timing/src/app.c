#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void task_a(void *arg)
{
    (void)arg;
    /* TODO: print a twice on a 50 ms cadence (see README) */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void task_b(void *arg)
{
    (void)arg;
    /* TODO: print b twice, staggered 25 ms behind task_a */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void done_task(void *arg)
{
    (void)arg;
    /* TODO: vTaskDelay then print DONE */
    for (;;) {
        vTaskDelay(1000);
    }
}

int main(void)
{
    uart_init();

    /* TODO: xTaskCreate task_a, task_b, done_task */
    vTaskStartScheduler();
    for (;;) {
    }
}