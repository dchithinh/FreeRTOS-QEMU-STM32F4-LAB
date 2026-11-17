#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static TaskHandle_t high_h;
static TaskHandle_t paused_h;

static void paused(void *arg)
{
    (void)arg;
    /* TODO: print resumed and DONE */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void high_task(void *arg)
{
    (void)arg;
    /* TODO: wait for notification from low, print high, resume paused */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void low_task(void *arg)
{
    (void)arg;
    /* TODO: print low, notify high */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void dummy_task(void *arg)
{
    (void)arg;
}

int main(void)
{
    uart_init();

    /* TODO: xTaskCreate dummy, vTaskDelete */
    /* TODO: xTaskCreate paused, vTaskSuspend paused */
    /* TODO: xTaskCreate high_task and low_task */
    (void)high_h;
    (void)paused_h;
    vTaskStartScheduler();
    for (;;) {
    }
}