#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"
#include "uart.h"

static StreamBufferHandle_t byte_stream;

static void writer_task(void *arg)
{
    (void)arg;
    /* TODO: xStreamBufferSend "abc" then delete */
}

static void reader_task(void *arg)
{
    (void)arg;
    /* TODO: xStreamBufferReceive, print payload, print DONE, delete */
}

int main(void)
{
    uart_init();

    /* TODO: xStreamBufferCreate; create writer and reader tasks */
    (void)byte_stream;
    vTaskStartScheduler();
    for (;;) {
    }
}