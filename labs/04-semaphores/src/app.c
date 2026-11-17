#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

static SemaphoreHandle_t count_sem;

static void taker(void *arg)
{
    (void)arg;
    /* TODO: xSemaphoreTake then print took */
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

    /* TODO: xSemaphoreCreateCounting max 2, initial 0; give twice before scheduler */
    /* TODO: xTaskCreate two takers and done_task */
    (void)count_sem;
    vTaskStartScheduler();
    for (;;) {
    }
}