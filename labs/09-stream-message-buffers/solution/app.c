#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"
#include "uart.h"

#define STREAM_SIZE 16

static StreamBufferHandle_t byte_stream;

static void writer_task(void *arg)
{
    (void)arg;
    static const char msg[] = "abc";
    xStreamBufferSend(byte_stream, msg, sizeof(msg) - 1, portMAX_DELAY);
    vTaskDelete(NULL);
}

static void reader_task(void *arg)
{
    (void)arg;
    char buf[8];

    size_t n = xStreamBufferReceive(byte_stream, buf, sizeof(buf) - 1, portMAX_DELAY);
    buf[n] = '\0';
    uart_puts(buf);
    uart_puts("\r\n");
    uart_puts("DONE\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    uart_init();

    byte_stream = xStreamBufferCreate(STREAM_SIZE, 1);
    xTaskCreate(writer_task, "wr", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    xTaskCreate(reader_task, "rd", configMINIMAL_STACK_SIZE * 2, NULL, 2, NULL);
    vTaskStartScheduler();
    for (;;) {
    }
}