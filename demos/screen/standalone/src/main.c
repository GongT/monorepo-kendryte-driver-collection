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
#include <lcd.h>
#include <sleep.h>
#include <stdio.h>
#include <sysctl.h>
#include "icon_generate.h"

uint32_t g_lcd_gram[LCD_X_MAX * LCD_Y_MAX / 2] __attribute__((aligned(128)));

static void io_set_power(void)
{
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
}

static void io_mux_init(void)
{
    sysctl_set_spi0_dvp_data(1);
}

int main(void)
{
    printf("lcd test\n");

    io_mux_init();
    io_set_power();

    lcd_init();

    lcd_clear(RED);
    msleep(500);
    lcd_clear(GREEN);
    msleep(500);
    lcd_clear(BLUE);
    msleep(500);
    lcd_clear(WHITE);

    lcd_set_direction(DIR_XY_LRUD);
    lcd_draw_picture(0, 0, 240, 160, g_lcd_gram);
    lcd_draw_string(16, 40, "Canaan", CYAN);

    lcd_draw_picture((240 - IMAGE_HEIGHT) / 2, (320 - IMAGE_WIDTH) / 2, IMAGE_HEIGHT, IMAGE_WIDTH, rgb_image);

    lcd_set_direction(DIR_XY_RLDU);
    lcd_draw_string(16, 40, "Kendryte K210", ORANGE);

    while(1)
        ;
}
