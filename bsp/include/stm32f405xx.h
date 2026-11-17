#pragma once

#include <stdint.h>

#define __IO volatile

#define EXTI0_IRQn   6
#define USART2_IRQn  38

#define USART2_BASE  0x40004400UL
#define RCC_BASE     0x40023800UL

#define RCC_APB1ENR_USART2EN (1UL << 17)

typedef struct {
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t BRR;
    __IO uint32_t CR1;
} USART_TypeDef;

#define USART_SR_TXE (1UL << 7)

#define USART_CR1_UE  (1UL << 13)
#define USART_CR1_TE  (1UL << 3)
#define USART_CR1_RE  (1UL << 2)

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t PLLCFGR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t AHB1RSTR;
    __IO uint32_t AHB2RSTR;
    __IO uint32_t AHB3RSTR;
    uint32_t      RESERVED0;
    __IO uint32_t APB1RSTR;
    __IO uint32_t APB2RSTR;
    uint32_t      RESERVED1[2];
    __IO uint32_t AHB1ENR;
    __IO uint32_t AHB2ENR;
    __IO uint32_t AHB3ENR;
    uint32_t      RESERVED2;
    __IO uint32_t APB1ENR;
    __IO uint32_t APB2ENR;
} RCC_TypeDef;

#define USART2 ((USART_TypeDef *)USART2_BASE)
#define RCC    ((RCC_TypeDef *)RCC_BASE)

/* NVIC pending (for EXTI0 / future IRQ BSP) */
#define NVIC_BASE          0xE000E000UL
#define NVIC_ISPR(n)       (*(__IO uint32_t *)(NVIC_BASE + 0x100UL + ((n) >> 5) * 4UL))
#define NVIC_SetPendingIRQ(irq) (NVIC_ISPR((irq) >> 5) = (1UL << ((irq) & 0x1FUL)))