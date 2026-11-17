#include <stdint.h>

/* Until FreeRTOS configASSERT / hook stubs are wired (task 4). */
void vAssertCalled(uint32_t ulLine, const char *pcFile)
{
    (void)ulLine;
    (void)pcFile;
    for (;;) {
    }
}