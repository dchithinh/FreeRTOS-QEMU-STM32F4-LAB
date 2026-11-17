#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

static StaticTask_t worker_tcb;
static StackType_t worker_stack[configMINIMAL_STACK_SIZE * 2];

static void worker_task(void *arg)
{
    (void)arg;
    /* TODO: print static; print DONE; delete */
}

int main(void)
{
    uart_init();

    /* TODO: xTaskCreateStatic for worker_task using worker_tcb and worker_stack */
    (void)worker_tcb;
    (void)worker_stack;
    vTaskStartScheduler();
    for (;;) {
    }
}