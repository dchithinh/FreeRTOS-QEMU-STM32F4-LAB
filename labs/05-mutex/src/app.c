#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

static SemaphoreHandle_t mtx;

static void first_locker(void *arg)
{
    (void)arg;
    /* TODO: take mutex, print locked, give mutex */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void second_locker(void *arg)
{
    (void)arg;
    /* TODO: take mutex, print unlocked, give mutex */
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

    /* TODO: xSemaphoreCreateMutex; create two locker tasks and done_task */
    (void)mtx;
    vTaskStartScheduler();
    for (;;) {
    }
}