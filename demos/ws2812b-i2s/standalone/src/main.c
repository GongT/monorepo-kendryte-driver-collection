#include <bsp.h>
#include <fpioa.h>
#include <gpiohs.h>
#include <stdint.h>
#include <sysctl.h>
#include <uarths.h>
#include "ws2812b.h"

#define WS_PIN 40
#define I2S_NUM I2S_DEVICE_1

ws2812_info *ws_info;

int main(void)
{
    sysctl_cpu_set_freq(500000000UL);
    uarths_init();

    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    ws2812_init_i2s(WS_PIN, I2S_NUM, I2S_CHANNEL_3);

    ws_info = ws2812_get_buf(6);

    printk("WS_I2S_TEST\n");
    while(1)
    {
        ws2812_set_data(ws_info, 0, 0x11, 0, 0);
        ws2812_send_data_i2s(I2S_NUM, 1, ws_info);
        printk("red\n");
        sleep(1);
        ws2812_set_data(ws_info, 0, 0, 0x11, 0);
        ws2812_send_data_i2s(I2S_NUM, 1, ws_info);
        printk("green\n");
        sleep(1);
        ws2812_set_data(ws_info, 0, 0, 0, 0x11);
        ws2812_send_data_i2s(I2S_NUM, 1, ws_info);
        printk("blue\n");
        sleep(1);
    }
}
