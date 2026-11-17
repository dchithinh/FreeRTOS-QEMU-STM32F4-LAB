#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static StaticTask_t worker_tcb;
static StackType_t worker_stack[configMINIMAL_STACK_SIZE * 2];

static void worker_task(void *arg)
{
    (void)arg;
    uart_puts("static\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    xTaskCreateStatic(worker_task, "work",
                      sizeof(worker_stack) / sizeof(StackType_t),
                      NULL, 2, worker_stack, &worker_tcb);
    vTaskStartScheduler();
    for (;;) {
    }
}