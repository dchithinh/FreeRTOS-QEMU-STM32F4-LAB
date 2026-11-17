#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static TaskHandle_t waiter_h;

static void notifier(void *arg)
{
    (void)arg;
    /* TODO: short delay then xTaskNotifyGive(waiter_h) */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void waiter(void *arg)
{
    (void)arg;
    /* TODO: ulTaskNotifyTake then print n1 */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void done_task(void *arg)
{
    (void)arg;
    /* TODO: delay then print DONE */
    for (;;) {
        vTaskDelay(1000);
    }
}

int main(void)
{
    uart_init();

    /* TODO: create waiter (save handle), notifier, done_task */
    (void)waiter_h;
    vTaskStartScheduler();
    for (;;) {
    }
}