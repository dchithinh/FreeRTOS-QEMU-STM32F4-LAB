#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "uart.h"

static TimerHandle_t slow_tmr;
static TimerHandle_t fast_tmr;
static volatile uint32_t fast_hits;
static TaskHandle_t worker_h;

static void slow_cb(TimerHandle_t xTimer)
{
    (void)xTimer;
    /* BUG: timer callbacks run in the timer service task — never block here. */
    vTaskDelay(pdMS_TO_TICKS(100));
}

static void fast_cb(TimerHandle_t xTimer)
{
    (void)xTimer;
    fast_hits++;
}

static void worker_task(void *arg)
{
    (void)arg;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(250));
        if (fast_hits >= 4U) {
            uart_puts("DONE\r\n");
            vTaskDelete(NULL);
            return;
        }
    }
}

int main(void)
{
    uart_init();

    slow_tmr = xTimerCreate("slow", pdMS_TO_TICKS(20), pdTRUE, NULL, slow_cb);
    fast_tmr = xTimerCreate("fast", pdMS_TO_TICKS(30), pdTRUE, NULL, fast_cb);
    configASSERT(slow_tmr != NULL && fast_tmr != NULL);

    xTaskCreate(worker_task, "work", configMINIMAL_STACK_SIZE * 2, NULL, 2, &worker_h);
    xTimerStart(slow_tmr, portMAX_DELAY);
    xTimerStart(fast_tmr, portMAX_DELAY);
    vTaskStartScheduler();
    for (;;) {
    }
}