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

static void tick_task(void *arg)
{
    TickType_t t;

    (void)arg;

    t = xTaskGetTickCount();
    vTaskDelayUntil(&t, 5);
    uart_puts("tick ");
    uart_put_u32(5U);
    uart_puts("\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(tick_task, "tick", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}