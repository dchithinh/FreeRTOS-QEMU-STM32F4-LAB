#include "uart.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f405xx.h"

/* 1 = "[  1234] line" on UART, 0 = bare text. */
#ifndef UART_TIMESTAMP_DEFAULT
#define UART_TIMESTAMP_DEFAULT 1
#endif

static int s_timestamp_on = UART_TIMESTAMP_DEFAULT;
/* Tracks whether the next byte starts a line, across separate uart_puts calls
 * (labs that print "abc" then "\r\n" must not get a timestamp in between). */
static int s_at_line_start = 1;

void uart_init(void)
{
    /* QEMU netduinoplus2 wires -serial stdio to USART1 (serial_hd 0). */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* 115200 @ ~84 MHz USART1 clock (168 MHz SYSCLK, APB2 /2); BRR ignored by QEMU. */
    USART1->BRR = 0x02D9U;
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void uart_timestamp_enable(int on)
{
    s_timestamp_on = on ? 1 : 0;
}

/* Raw TX: no critical section, no timestamp. Used only while already locked. */
static void raw_putc(char c)
{
    while ((USART1->SR & USART_SR_TXE) == 0U) {
    }
    USART1->DR = (uint32_t)(uint8_t)c;
}

static void raw_puts(const char *s)
{
    while (*s) {
        raw_putc(*s++);
    }
}

/* Unsigned decimal, zero padded to WIDTH (right aligned). */
static void raw_put_u32(uint32_t v, uint32_t width)
{
    char buf[10];
    uint32_t n = 0;

    do {
        buf[n++] = (char)('0' + (v % 10U));
        v /= 10U;
    } while (v != 0U);

    while (n < width) {
        raw_putc(' ');
        width--;
    }
    while (n > 0U) {
        raw_putc(buf[--n]);
    }
}

static uint32_t current_tick_hz(void)
{
    /* The BSP is compiled once with the default config, but a lab may run its
     * kernel at a different configTICK_RATE_HZ (lab 24). Read the live SysTick
     * reload so timestamps stay correct either way. */
    uint32_t reload_plus_one = SysTick->LOAD + 1U;

    if (reload_plus_one <= 1U) {
        return (uint32_t)configTICK_RATE_HZ; /* scheduler not started yet */
    }
    return (uint32_t)(configCPU_CLOCK_HZ / reload_plus_one);
}

static uint32_t tick_to_ms(TickType_t ticks)
{
    uint32_t hz = current_tick_hz();

    if (hz == 0U) {
        hz = 1U;
    }
    /* 64-bit avoids overflow past ~24 days at 1 kHz. */
    return (uint32_t)(((uint64_t)ticks * 1000ULL) / (uint64_t)hz);
}

static void emit_timestamp(void)
{
    raw_putc('[');
    raw_put_u32(tick_to_ms(xTaskGetTickCount()), 6U);
    raw_puts("] ");
}

void uart_puts(const char *s)
{
    taskENTER_CRITICAL();
    while (*s) {
        char c = *s++;

        if (s_at_line_start && s_timestamp_on) {
            emit_timestamp();
            s_at_line_start = 0;
        }
        raw_putc(c);
        s_at_line_start = (c == '\n');
    }
    taskEXIT_CRITICAL();
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
