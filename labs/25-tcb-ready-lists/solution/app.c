#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static volatile uint32_t done_mask;

static void pri_task(void *arg)
{
    uint32_t id = (uint32_t)(uintptr_t)arg;

    uart_puts((const char *[]) { "", "1\r\n", "2\r\n", "3\r\n" }[id]);
    done_mask |= 1U << id;
    vTaskDelete(NULL);
}

static void waiter_task(void *arg)
{
    (void)arg;

    while (done_mask != 0x0EU) {
        vTaskDelay(1);
    }
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreate(waiter_task, "wait", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    xTaskCreate(pri_task, "p1", configMINIMAL_STACK_SIZE * 2, (void *)(uintptr_t)1U, 1, NULL);
    xTaskCreate(pri_task, "p2", configMINIMAL_STACK_SIZE * 2, (void *)(uintptr_t)2U, 2, NULL);
    xTaskCreate(pri_task, "p3", configMINIMAL_STACK_SIZE * 2, (void *)(uintptr_t)3U, 3, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}