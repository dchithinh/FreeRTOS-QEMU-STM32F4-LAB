#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "uart.h"

static TimerHandle_t reload_tmr;
static uint32_t tick_count;

static void tick_cb(TimerHandle_t xTimer)
{
    (void)xTimer;
    /* TODO: count expirations; print tick; after 2, stop timer and print DONE */
}

int main(void)
{
    uart_init();

    /* TODO: xTimerCreate 50 ms auto-reload; xTimerStart before vTaskStartScheduler */
    (void)reload_tmr;
    (void)tick_count;
    vTaskStartScheduler();
    for (;;) {
    }
}