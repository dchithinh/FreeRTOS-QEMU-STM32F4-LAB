#pragma once

#include <stdint.h>

void bsp_irq_init(uint32_t nvic_priority);
void bsp_irq_pend(void);
void BSP_IRQHandler(void);