#pragma once

void uart_init(void);
void uart_puts(const char *s);
void uart_printf(const char *fmt, ...);

/* Prefix every line with "[  <ms>] " (kernel tick converted to ms).
 * On by default; call with 0 to get plain output. */
void uart_timestamp_enable(int on);
