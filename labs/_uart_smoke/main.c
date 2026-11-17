#include "irq.h"
#include "uart.h"

static volatile uint32_t irq_done;

void BSP_IRQHandler(void)
{
    uart_puts("IRQ\r\n");
    irq_done = 1U;
}

int main(void)
{
    uart_init();
    bsp_irq_init(5U);
    bsp_irq_pend();
    while (irq_done == 0U) {
    }
    uart_puts("DONE\r\n");
    for (;;) {
    }
}