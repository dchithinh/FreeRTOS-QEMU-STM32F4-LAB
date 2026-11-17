#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "uart.h"

#define BIT_ONE (1UL << 0)
#define BIT_TWO (1UL << 1)
#define SYNC_BITS (BIT_ONE | BIT_TWO)

static EventGroupHandle_t sync_grp;

static void barrier_task(void *arg)
{
    (void)arg;
    /* TODO: xEventGroupSync with your bit and SYNC_BITS; one task prints sync */
    for (;;) {
        vTaskDelay(1000);
    }
}

static void done_task(void *arg)
{
    (void)arg;
    /* TODO: delay then print DONE */
    for (;;) {
        vTaskDelay(1000);
    }
}

int main(void)
{
    uart_init();

    /* TODO: xEventGroupCreate; two barrier tasks (bits 1 and 2) and done_task */
    (void)sync_grp;
    vTaskStartScheduler();
    for (;;) {
    }
}