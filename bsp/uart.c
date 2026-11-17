#include "uart.h"

#include <stdarg.h>
#include <stdio.h>

#include "stm32f405xx.h"

void uart_init(void)
{
    /* QEMU netduinoplus2 wires -serial stdio to USART1 (serial_hd 0). */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* 115200 @ ~84 MHz USART1 clock (168 MHz SYSCLK, APB2 /2); BRR ignored by QEMU. */
    USART1->BRR = 0x02D9U;
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void uart_puts(const char *s)
{
    while (*s) {
        while ((USART1->SR & USART_SR_TXE) == 0U) {
        }
        USART1->DR = (uint32_t)(uint8_t)*s++;
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