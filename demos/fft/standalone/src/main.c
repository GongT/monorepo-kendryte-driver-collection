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
#include <encoding.h>
#include <fft.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysctl.h>

#define FFT_N 512U
#define FFT_FORWARD_SHIFT 0x0U
#define FFT_BACKWARD_SHIFT 0x1ffU
#define PI 3.14159265358979323846

int16_t real[FFT_N];
int16_t imag[FFT_N];
float power[FFT_N];
float angel[FFT_N];
uint64_t fft_out_data[FFT_N / 2];
uint64_t buffer_input[FFT_N];
uint64_t buffer_output[FFT_N];
uint64_t cycle[FFT_DIR_MAX];

uint16_t get_bit1_num(uint32_t data)
{
    uint16_t num;
    for(num = 0; data; num++)
        data &= data - 1;
    return num;
}

int main(void)
{
    int32_t i;
    float tempf1[3];
    fft_data_t *output_data;
    fft_data_t *input_data;
    uint16_t bit1_num = get_bit1_num(FFT_FORWARD_SHIFT);
    complex_hard_t data[FFT_N] = {0};
    for(i = 0; i < FFT_N; i++)
    {
        tempf1[0] = 0.3 * cosf(2 * PI * i / FFT_N + PI / 3) * 256;
        tempf1[1] = 0.1 * cosf(16 * 2 * PI * i / FFT_N - PI / 9) * 256;
        tempf1[2] = 0.5 * cosf((19 * 2 * PI * i / FFT_N) + PI / 6) * 256;
        data[i].real = (int16_t)(tempf1[0] + tempf1[1] + tempf1[2] + 10);
        data[i].imag = (int16_t)0;
    }
    for(int i = 0; i < FFT_N / 2; ++i)
    {
        input_data = (fft_data_t *)&buffer_input[i];
        input_data->R1 = data[2 * i].real;
        input_data->I1 = data[2 * i].imag;
        input_data->R2 = data[2 * i + 1].real;
        input_data->I2 = data[2 * i + 1].imag;
    }
    cycle[FFT_DIR_FORWARD] = read_cycle();
    fft_complex_uint16_dma(DMAC_CHANNEL0, DMAC_CHANNEL1, FFT_FORWARD_SHIFT, FFT_DIR_FORWARD, buffer_input, FFT_N, buffer_output);
    cycle[FFT_DIR_FORWARD] = read_cycle() - cycle[FFT_DIR_FORWARD];
    for(i = 0; i < FFT_N / 2; i++)
    {
        output_data = (fft_data_t *)&buffer_output[i];
        data[2 * i].imag = output_data->I1;
        data[2 * i].real = output_data->R1;
        data[2 * i + 1].imag = output_data->I2;
        data[2 * i + 1].real = output_data->R2;
    }

    for(i = 0; i < FFT_N; i++)
    {
        power[i] = sqrt(data[i].real * data[i].real + data[i].imag * data[i].imag) * 2;
    }

    printf("\n[fft real][fft imag]\n");
    for(i = 0; i < FFT_N / 2; i++)
        printf("%3d: %7d %7d\n",
               i, data[i].real, data[i].imag);

    printf("\nphase:\n");
    for(i = 0; i < FFT_N / 2; i++)
    {
        angel[i] = atan2(data[i].imag, data[i].real);
        printf("%3d : %f\n", i, angel[i] * 180 / PI);
    }

    for(int i = 0; i < FFT_N / 2; ++i)
    {
        input_data = (fft_data_t *)&buffer_input[i];
        input_data->R1 = data[2 * i].real;
        input_data->I1 = data[2 * i].imag;
        input_data->R2 = data[2 * i + 1].real;
        input_data->I2 = data[2 * i + 1].imag;
    }
    cycle[FFT_DIR_BACKWARD] = read_cycle();
    fft_complex_uint16_dma(DMAC_CHANNEL0, DMAC_CHANNEL1, FFT_BACKWARD_SHIFT, FFT_DIR_BACKWARD, buffer_input, FFT_N, buffer_output);
    cycle[FFT_DIR_BACKWARD] = read_cycle() - cycle[FFT_DIR_BACKWARD];
    for(i = 0; i < FFT_N / 2; i++)
    {
        output_data = (fft_data_t *)&buffer_output[i];
        data[2 * i].imag = output_data->I1;
        data[2 * i].real = output_data->R1;
        data[2 * i + 1].imag = output_data->I2;
        data[2 * i + 1].real = output_data->R2;
    }

    printf("[fft test] [%d bytes] forward time = %ld us, backward time = %ld us\n",
           FFT_N,
           cycle[FFT_DIR_FORWARD] / (sysctl_clock_get_freq(SYSCTL_CLOCK_CPU) / 1000000),
           cycle[FFT_DIR_BACKWARD] / (sysctl_clock_get_freq(SYSCTL_CLOCK_CPU) / 1000000));
    while(1)
        ;
    return 0;
}
