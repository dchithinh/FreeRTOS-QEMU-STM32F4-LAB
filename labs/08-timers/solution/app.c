#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "uart.h"

static TimerHandle_t reload_tmr;
static uint32_t tick_count;

static void tick_cb(TimerHandle_t xTimer)
{
    (void)xTimer;
    tick_count++;
    uart_puts("tick\r\n");
    if (tick_count >= 2) {
        xTimerStop(reload_tmr, 0);
        uart_puts("DONE\r\n");
    }
}

int main(void)
{
    uart_init();

    reload_tmr = xTimerCreate("t", pdMS_TO_TICKS(50), pdTRUE, NULL, tick_cb);
    xTimerStart(reload_tmr, portMAX_DELAY);
    vTaskStartScheduler();
    for (;;) {
    }
}