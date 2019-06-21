#include <bsp.h>
#include <fpioa.h>
#include <gpiohs.h>
#include <stdint.h>
#include <sysctl.h>
#include <uarths.h>
#include <ws2812b.h>

#define WS_PIN 40
#define SPI_NUM SPI_DEVICE_0

ws2812_info *ws_info;

int main(void)
{
    sysctl_cpu_set_freq(500000000UL);
    uarths_init();

    ws2812_init_spi(WS_PIN, SPI_NUM);

    ws_info = ws2812_get_buf(6);

    printk("WS_SPI_TEST\n");
    while(1)
    {
        ws2812_set_data(ws_info, 0, 0, 0, 0x11);
        ws2812_send_data_spi(SPI_NUM, 1, ws_info);
        printk("blue\n");
        sleep(1);
        ws2812_set_data(ws_info, 0, 0, 0x11, 0);
        ws2812_send_data_spi(SPI_NUM, 1, ws_info);
        printk("green\n");
        sleep(1);
        ws2812_set_data(ws_info, 0, 0x11, 0, 0);
        ws2812_send_data_spi(SPI_NUM, 1, ws_info);
        printk("red\n");
        sleep(1);
    }
}
