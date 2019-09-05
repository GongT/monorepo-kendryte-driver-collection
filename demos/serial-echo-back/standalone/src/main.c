#include <stdio.h>
#include <uart.h>
#include <unistd.h>

int main()
{
    uart_init(UART_DEVICE_1);
    uart_configure(UART_DEVICE_1, 115200, DMAC_TRANS_WIDTH_8, UART_STOP_1, UART_PARITY_NONE);

    uart_send_data(UART_DEVICE_1, "Hello World!", 12);

    char c;
    while(1)
    {
        if(uart_receive_data(UART_DEVICE_1, &c, 1))
        {
            uart_send_data(UART_DEVICE_1, &c, 1);
        }
    }
    return 1;
}
