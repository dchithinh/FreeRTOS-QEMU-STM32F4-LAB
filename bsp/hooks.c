#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

void vApplicationMallocFailedHook(void)
{
    uart_puts("HOOK MALLOC_FAILED\r\n");
    for (;;) {
    }
}

void vApplicationStackOverflowHook(TaskHandle_t t, char *n)
{
    (void)t;
    (void)n;
    uart_puts("HOOK STACK_OVERFLOW\r\n");
    for (;;) {
    }
}

void vApplicationGetIdleTaskMemory(StaticTask_t **tcb, StackType_t **stack, uint32_t *size)
{
    static StaticTask_t idle_tcb;
    static StackType_t idle_stack[configMINIMAL_STACK_SIZE];

    *tcb = &idle_tcb;
    *stack = idle_stack;
    *size = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **tcb, StackType_t **stack, uint32_t *size)
{
    static StaticTask_t tmr_tcb;
    static StackType_t tmr_stack[configTIMER_TASK_STACK_DEPTH];

    *tcb = &tmr_tcb;
    *stack = tmr_stack;
    *size = configTIMER_TASK_STACK_DEPTH;
}