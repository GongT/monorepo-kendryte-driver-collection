#include <bsp.h>
#include <fpioa.h>
#include <gpiohs.h>
#include <stdint.h>
#include <sysctl.h>
#include <uarths.h>
#include "ws2812b.h"

#define I2S_NUM I2S_DEVICE_1

ws2812_info *ws_info;

int main(void)
{
    sysctl_cpu_set_freq(500000000UL);
    uarths_init();

    uint32_t pll2_rate = 45158400UL;
    sysctl_pll_set_freq(SYSCTL_PLL2, pll2_rate);
    uint32_t sample_rate = pll2_rate / (2 * 32 * 2) - 1;
    i2s_set_sample_rate(I2S_NUM, sample_rate);
    i2s_init(I2S_NUM, I2S_TRANSMITTER, 0x3 << 2 * I2S_CHANNEL_3);
    i2s_tx_channel_config(I2S_NUM, I2S_CHANNEL_3,
                          RESOLUTION_32_BIT, SCLK_CYCLES_32,
                          TRIGGER_LEVEL_4,
                          LEFT_JUSTIFYING_MODE);

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
