#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

#define LOOPS_PER_TASK 1000U
#define EXPECT_PER_TEST  (2U * LOOPS_PER_TASK)

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
static volatile uint32_t done_mask;
static SemaphoreHandle_t mtx;

typedef enum {
    PROT_CRITICAL,
    PROT_SUSPEND,
    PROT_MUTEX,
} protect_mode_t;

static protect_mode_t mode;

static void inc_task(void *arg)
{

    for (uint32_t i = 0; i < LOOPS_PER_TASK; i++) {
        if (mode == PROT_CRITICAL) {
            /* TODO: taskENTER_CRITICAL() / taskEXIT_CRITICAL() around counter++ */
            counter++;
        } else if (mode == PROT_SUSPEND) {
            /* TODO: vTaskSuspendAll(), counter++, xTaskResumeAll() */
            counter++;
        } else {
            /* TODO: xSemaphoreTake(mtx), counter++, xSemaphoreGive(mtx) */
            counter++;
        }
    }
    done_mask |= (uintptr_t)arg;
    vTaskDelete(NULL);
}

static uint32_t run_pair_test(protect_mode_t m)
{
    mode = m;
    counter = 0;
    done_mask = 0;
    xTaskCreate(inc_task, "a", configMINIMAL_STACK_SIZE * 2, (void *)(uintptr_t)1U, 2, NULL);
    xTaskCreate(inc_task, "b", configMINIMAL_STACK_SIZE * 2, (void *)(uintptr_t)2U, 2, NULL);
    while (done_mask != 3U) {
        vTaskDelay(1);
    }
    return counter;
}

static void runner_task(void *arg)
{
    uint32_t c1;
    uint32_t c2;
    uint32_t c3;

    (void)arg;

    c1 = run_pair_test(PROT_CRITICAL);
    c2 = run_pair_test(PROT_SUSPEND);
    c3 = run_pair_test(PROT_MUTEX);

    uart_puts("crit=");
    uart_put_u32(c1);
    uart_puts(" susp=");
    uart_put_u32(c2);
    uart_puts(" mtx=");
    uart_put_u32(c3);
    uart_puts("\r\n");
    if (c1 == EXPECT_PER_TEST && c2 == EXPECT_PER_TEST && c3 == EXPECT_PER_TEST) {
        uart_puts("ok\r\n");
        uart_puts("DONE\r\n");
    }
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    mtx = xSemaphoreCreateMutex();
    configASSERT(mtx != NULL);

    xTaskCreate(runner_task, "run", configMINIMAL_STACK_SIZE * 3, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}