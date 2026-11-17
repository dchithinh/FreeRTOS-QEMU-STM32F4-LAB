#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void blow_stack(uint32_t depth)
{
    volatile uint8_t frame[80];

    for (uint32_t i = 0; i < sizeof frame; i++) {
        frame[i] = (uint8_t)depth;
    }
    if (depth < 3U) {
        blow_stack(depth + 1U);
    }
    /* Deepest call overwrites the method-2 canary; blocking forces a context switch. */
    vTaskDelay(portMAX_DELAY);
}

static void heavy_task(void *arg)
{
    (void)arg;
    blow_stack(0U);
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(heavy_task, "heavy", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}