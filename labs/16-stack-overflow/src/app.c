#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void blow_stack(uint32_t depth)
{
    volatile uint8_t frame[64];

    frame[0] = (uint8_t)depth;
    if (depth < 6U) {
        blow_stack(depth + 1U);
    } else {
        /* Deep locals on minimal stack — same hook as configCHECK (bsp/hooks.c). */
        vApplicationStackOverflowHook(NULL, "heavy");
    }
}

static void heavy_task(void *arg)
{
    (void)arg;
    blow_stack(0);
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