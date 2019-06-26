/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <dmac.h>
#include <fft.h>
#include <fpioa.h>
#include <i2s.h>
#include <lcd.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysctl.h>
#include <uarths.h>
#include "draw_image.h"

#define FFT_N 512U
#define FFT_FORWARD_SHIFT 0x0U
#define SAMPLE_RATE 38640

uint16_t get_bit1_num(uint32_t data)
{
    uint16_t num;
    for(num = 0; data; num++)
        data &= data - 1;
    return num;
}

int main(void)
{
    sysctl_set_spi0_dvp_data(1);

    uint32_t g_index;
    uint32_t g_tx_len;

    int16_t real[FFT_N];
    int16_t imag[FFT_N];
    uint64_t fft_out_data[FFT_N / 2];

    sysctl_pll_set_freq(SYSCTL_PLL0, 480000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);

    uarths_init();
    printf("LCD init.\n");
    lcd_init();
    lcd_set_direction(DIR_XY_RLDU);
    init_bars();

    i2s_init(I2S_DEVICE_0, I2S_RECEIVER, 0x3);

    i2s_set_sample_rate(I2S_DEVICE_0, SAMPLE_RATE);

    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_0,
                          RESOLUTION_16_BIT, SCLK_CYCLES_32,
                          TRIGGER_LEVEL_4, STANDARD_MODE);

    uint32_t rx_buf[FFT_N * 2];
    uint64_t buffer_input[FFT_N];
    memset(buffer_input, 0, sizeof(buffer_input));
    float hard_power_avg[FFT_N];
    memset(hard_power_avg, 0, sizeof(hard_power_avg));

    while(1)
    {
        i2s_receive_data_dma(I2S_DEVICE_0, rx_buf, FFT_N * 2, DMAC_CHANNEL3);

        for(int i = 0; i < FFT_N; i += 2)
        {
            fft_data_t *input_data = (fft_data_t *)&buffer_input[i];
            input_data->R1 = rx_buf[i];
            input_data->R2 = rx_buf[i + 1];
        }

        uint64_t buffer_output[FFT_N];
        fft_complex_uint16_dma(DMAC_CHANNEL0, DMAC_CHANNEL1, FFT_FORWARD_SHIFT, FFT_DIR_FORWARD, buffer_input, FFT_N, buffer_output);

        complex_hard_t *buffer_output_complex_itr = (complex_hard_t *)buffer_output;

        // printk("hard power: ");
        for(int i = 0; i < FFT_N; i++)
        {
            float hard_power;
            hard_power = sqrt(buffer_output_complex_itr[i].real * buffer_output_complex_itr[i].real + buffer_output_complex_itr[i].imag * buffer_output_complex_itr[i].imag);
            //Convert to dBFS
            hard_power = 20 * log(2 * hard_power / FFT_N);
            if(hard_power < 0)
                hard_power = 0;

            hard_power_avg[i] = (hard_power_avg[i] * 2 + hard_power) / 3;
            // printk("%x ", hard_power[i] );
        }
        // printk("\n");
        update_bars(hard_power_avg, 140, FFT_N);
    }

    return 0;
}
