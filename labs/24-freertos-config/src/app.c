#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static void uart_put_u32(uint32_t v)
{
    char buf[11];
    int i = 10;

    if (v == 0U) {
        uart_puts("0");
        return;
    }
    buf[10] = '\0';
    while (v > 0U && i > 0) {
        buf[--i] = (char)('0' + (v % 10U));
        v /= 10U;
    }
    uart_puts(buf + i);
}

static void demo_task(void *arg)
{
    (void)arg;

    TickType_t t0 = xTaskGetTickCount();

    for (int i = 0; i < 10; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    uart_puts("ticks=");
    uart_put_u32((uint32_t)(xTaskGetTickCount() - t0));
    uart_puts("\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(demo_task, "demo", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}