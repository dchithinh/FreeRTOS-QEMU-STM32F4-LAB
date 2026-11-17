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
    EventBits_t my_bit = (EventBits_t)(uintptr_t)arg;
    xEventGroupSync(sync_grp, my_bit, SYNC_BITS, portMAX_DELAY);
    if (my_bit == BIT_ONE) {
        uart_puts("sync\r\n");
    }
    vTaskDelete(NULL);
}

static void done_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(50));
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    sync_grp = xEventGroupCreate();
    xTaskCreate(barrier_task, "b1", configMINIMAL_STACK_SIZE * 2, (void *)(uintptr_t)BIT_ONE, 3,
                NULL);
    xTaskCreate(barrier_task, "b2", configMINIMAL_STACK_SIZE * 2, (void *)(uintptr_t)BIT_TWO, 2,
                NULL);
    xTaskCreate(done_task, "d", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}