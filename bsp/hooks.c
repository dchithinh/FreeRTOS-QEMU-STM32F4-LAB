#include "FreeRTOS.h"
#include "task.h"
#include "stm32f405xx.h"
#include "uart.h"

static void hook_uart_puts_isr_safe(const char *s)
{
    while (*s) {
        while ((USART1->SR & USART_SR_TXE) == 0U) {
        }
        USART1->DR = (uint32_t)(uint8_t)*s++;
    }
}

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
    /* Called from the scheduler during a context switch — no task critical sections. */
    hook_uart_puts_isr_safe("HOOK STACK_OVERFLOW\r\n");
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