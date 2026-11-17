#include "uart.h"

int main(void)
{
    uart_init();
    uart_puts("HELLO\r\nDONE\r\n");
    for (;;) {
    }
}