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

void update_image(float *hard_power, float pw_max, uint32_t *pImage, uint32_t color, uint32_t bkg_color);

int main(void)
{
    sysctl_set_spi0_dvp_data(1);

    complex_hard_t data_hard[FFT_N] = {0};
    fft_data_t *output_data;

    uint32_t g_index;
    uint32_t g_tx_len;

    uint32_t g_lcd_gram[LCD_X_MAX * LCD_Y_MAX] __attribute__((aligned(64)));

    int16_t real[FFT_N];
    int16_t imag[FFT_N];
    float hard_power[FFT_N];
    uint64_t fft_out_data[FFT_N / 2];
    uint64_t buffer_output[FFT_N];

    sysctl_pll_set_freq(SYSCTL_PLL0, 480000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);

    uarths_init();

    lcd_init();
    lcd_set_direction(DIR_XY_RLDU);
    lcd_clear(BLACK);

    i2s_init(I2S_DEVICE_0, I2S_RECEIVER, 0x3);

    i2s_set_sample_rate(I2S_DEVICE_0, SAMPLE_RATE);

    i2s_rx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_0,
                          RESOLUTION_16_BIT, SCLK_CYCLES_32,
                          TRIGGER_LEVEL_4, STANDARD_MODE);

    uint32_t rx_buf[FFT_N * 2];
    uint64_t buffer_input[FFT_N];
    for(int i = 0; i < FFT_N; i++)
    {
        buffer_input[i] = 0;
    }

    while(1)
    {
        i2s_receive_data_dma(I2S_DEVICE_0, rx_buf, FFT_N * 2, DMAC_CHANNEL3);

        fft_data_t *input_data;
        for(int i = 0; i < FFT_N; i += 2)
        {
            input_data = (fft_data_t *)&buffer_input[i];
            input_data->R1 = rx_buf[i];
            input_data->I1 = 0;
            input_data->R2 = rx_buf[i + 1];
            input_data->I2 = 0;
        }

        fft_complex_uint16_dma(DMAC_CHANNEL0, DMAC_CHANNEL1, FFT_FORWARD_SHIFT, FFT_DIR_FORWARD, buffer_input, FFT_N, buffer_output);

        for(int i = 0; i < FFT_N / 2; i++)
        {
            output_data = (fft_data_t *)&buffer_output[i];
            data_hard[2 * i].imag = output_data->I1;
            data_hard[2 * i].real = output_data->R1;
            data_hard[2 * i + 1].imag = output_data->I2;
            data_hard[2 * i + 1].real = output_data->R2;
        }

        float pmax = 10;
        for(i = 0; i < FFT_N; i++)
        {
            hard_power[i] = sqrt(data_hard[i].real * data_hard[i].real + data_hard[i].imag * data_hard[i].imag);

            //Convert to dBFS
            hard_power[i] = 20 * log(2 * hard_power[i] / FFT_N);

            if(hard_power[i] > pmax)
                pmax = hard_power[i];
        }

        update_image(hard_power, 140 /*MAX range dBFS*/, g_lcd_gram, RED, BLACK);
        lcd_draw_picture(0, 0, LCD_X_MAX, LCD_Y_MAX, g_lcd_gram);
    }

    return 0;
}

void update_image(float *hard_power, float pw_max, uint32_t *pImage, uint32_t color, uint32_t bkg_color)
{

    uint32_t bcolor = (bkg_color << 16) | bkg_color;
    uint32_t fcolor = (color << 16) | color;

    int h[80];

    int i = 0;
    int x = 0;

    for(int i = 0; i < 80; ++i)
    {
        h[i] = 120 * (hard_power[i]) / pw_max;

        if(h[i] > 120)
            h[i] = 120;
        if(h[i] < 0)
            h[i] = 0;
    }

    for(i = 0; i < 53; ++i) // 53* 38640/512 => ~4000Hz
    {
        x = i * 3;
        for(int y = 0; y < 120; ++y)
        {
            if(y < h[i + 2])
            {
                pImage[y + x * 240] = fcolor;
                pImage[y + 120 + x * 240] = fcolor;
                pImage[y + (x + 1) * 240] = fcolor;
                pImage[y + 120 + (x + 1) * 240] = fcolor;
                pImage[y + (x + 2) * 240] = bcolor;
                pImage[y + 120 + (x + 2) * 240] = bcolor;
            } else
            {
                pImage[y + x * 240] = bcolor;
                pImage[y + 120 + x * 240] = bcolor;
                pImage[y + (x + 1) * 240] = bcolor;
                pImage[y + 120 + (x + 1) * 240] = bcolor;
                pImage[y + (x + 2) * 240] = bcolor;
                pImage[y + 120 + (x + 2) * 240] = bcolor;
            }
        }
    }
}
