#include "irq.h"

#include "stm32f405xx.h"

void bsp_irq_init(uint32_t nvic_priority)
{
    NVIC_SetPriority(EXTI0_IRQn, nvic_priority);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

void bsp_irq_pend(void)
{
    NVIC_SetPendingIRQ(EXTI0_IRQn);
}

__attribute__((weak)) void BSP_IRQHandler(void)
{
}