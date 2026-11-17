#include "uart.h"

#include <stdarg.h>
#include <stdio.h>

#include "stm32f405xx.h"

void uart_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* 115200 baud @ 16 MHz APB1 (QEMU netduinoplus2 HSI default) */
    USART2->BRR = 0x008BU;
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void uart_puts(const char *s)
{
    while (*s) {
        while ((USART2->SR & USART_SR_TXE) == 0U) {
        }
        USART2->DR = (uint32_t)(uint8_t)*s++;
    }
}

void uart_printf(const char *fmt, ...)
{
    char buf[128];
    va_list ap;

    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    uart_puts(buf);
}