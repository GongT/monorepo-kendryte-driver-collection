#include <dmac.h>
#include <stddef.h>
#include <stdint.h>
#include <syscalls.h>
#include <sysctl.h>
#include <uart.h>
#include "redirect-stdio.h"

#if STDIO_UART_DMAC_CHANNEL >= 0
static void send(const char *str, size_t size)
{
    uart_send_data_dma(STDIO_UART_NUM, STDIO_UART_DMAC_CHANNEL, (uint8_t *)str, size);
}
#else
static void send(const char *str, size_t size)
{
    uart_send_data(STDIO_UART_NUM, str, size);
}
#endif

#if STDIO_UART_CACHE_SIZE > 0
static char uart_send_cache[STDIO_UART_CACHE_SIZE];
static uint8_t send_cache_index = 0;
void push_send(char c)
{
    uart_send_cache[send_cache_index] = c;
    send_cache_index++;

    if(send_cache_index == STDIO_UART_CACHE_SIZE || c == '\n' || c == '\r')
    {
        send(uart_send_cache, send_cache_index);
        send_cache_index = 0;
    }
}
#else
void push_send(char c)
{
    send(&c, 1);
}
#endif

void redirect_stdio()
{
    uart_init(STDIO_UART_NUM);
    uart_config(STDIO_UART_NUM, STDIO_UART_BAUDRATE, STDIO_UART_BITWIDTH, STDIO_UART_STOP,
                STDIO_UART_PARITY);
    uart_set_receive_trigger(STDIO_UART_NUM, UART_RECEIVE_FIFO_8);
    sys_register_getchar([]() -> int {
        char c = 0;
        int r = uart_receive_data(STDIO_UART_NUM, &c, 1);
        return r == 0 ? EOF : c;
    });
    sys_register_putchar([](char c) -> int {
        push_send(c);
        return c;
    });
    send("\n"
         "\x1B"
         "c\r",
         4);
}
