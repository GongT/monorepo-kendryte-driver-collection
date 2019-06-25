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
#include <fpioa.h>
#include <i2s.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysctl.h>
#include <uarths.h>
#include "music.h"

#define RX_BUF_THRESHOLD (1024 * 8)
#define MAX_BUF_LEN (1024 * 16)

void io_mux_init(void)
{
    fpioa_set_function(33, FUNC_I2S0_OUT_D1);
    fpioa_set_function(35, FUNC_I2S0_SCLK);
    fpioa_set_function(34, FUNC_I2S0_WS);
}

int main(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, 320000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL1, 160000000UL);
    sysctl_pll_set_freq(SYSCTL_PLL2, 45158400UL);
    uarths_init();

    io_mux_init();
    printf("I2S1 INT test...\n");

    i2s_init(I2S_DEVICE_0, I2S_TRANSMITTER, 0x0C);

    i2s_tx_channel_config(I2S_DEVICE_0, I2S_CHANNEL_1,
                          RESOLUTION_16_BIT, SCLK_CYCLES_32,
                          /*TRIGGER_LEVEL_1*/ TRIGGER_LEVEL_4,
                          RIGHT_JUSTIFYING_MODE);

    while(1)
    {
        i2s_play(I2S_DEVICE_0,
                 DMAC_CHANNEL0, (uint8_t *)test_pcm, sizeof(test_pcm), 1024, 16, 2);
    }
    return 0;
}
