#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

#define LOOPS_PER_TASK 10000U
#define EXPECTED       (2U * LOOPS_PER_TASK)

static void uart_put_u32(uint32_t v)
{
    char buf[11];
    char *p = buf + sizeof(buf);

    *--p = '\0';
    if (v == 0U) {
        uart_puts("0");
        return;
    }
    while (v > 0U) {
        *--p = (char)('0' + (v % 10U));
        v /= 10U;
    }
    uart_puts(p);
}

static volatile uint32_t counter;
static SemaphoreHandle_t mtx;

static void inc_task(void *arg)
{
    (void)arg;

    for (uint32_t i = 0; i < LOOPS_PER_TASK; i++) {
        xSemaphoreTake(mtx, portMAX_DELAY);
        counter++;
        xSemaphoreGive(mtx);
    }
    vTaskDelete(NULL);
}

static void report_task(void *arg)
{
    (void)arg;

    vTaskDelay(pdMS_TO_TICKS(200));
    uart_put_u32(counter);
    uart_puts("\r\n");
    if (counter == EXPECTED) {
        uart_puts("DONE\r\n");
    }
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    mtx = xSemaphoreCreateMutex();
    configASSERT(mtx != NULL);

    counter = 0;
    xTaskCreate(inc_task, "a", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(inc_task, "b", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(report_task, "rpt", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}