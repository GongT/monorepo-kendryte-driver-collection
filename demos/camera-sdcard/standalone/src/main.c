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
#include <camera.h>
#include <dvp.h>
#include <ff.h>
#include <fpioa.h>
#include <gpiohs.h>
#include <lcd.h>
#include <plic.h>
#include <rgb2bmp.h>
#include <sdcard.h>
#include <stdio.h>
#include <string.h>
#include <sysctl.h>
#include <uarths.h>

/* SPI and DMAC usage
 *
 * SPI0 - LCD
 * SPI1 - SD card
 * SPI2 - unused
 * SPI3 - Flash
 *
 * DMAC Channel 0 - LCD
 * DMAC Channel 1 - SD card
 *
 */

#define KEY_GPIONUM 11

uint32_t g_lcd_gram0[38400] __attribute__((aligned(64)));
uint32_t g_lcd_gram1[38400] __attribute__((aligned(64)));

volatile uint8_t g_dvp_finish_flag;
volatile uint8_t g_ram_mux;
volatile uint8_t g_save_flag;

void irq_key(void *gp)
{
    g_save_flag = 1;
}

static int on_irq_dvp(void *ctx)
{
    if(dvp_get_interrupt(DVP_STS_FRAME_FINISH))
    {
        /* switch gram */
        dvp_set_display_addr(g_ram_mux ? (uint32_t)g_lcd_gram0 : (uint32_t)g_lcd_gram1);

        dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
        g_dvp_finish_flag = 1;
    } else
    {
        if(g_dvp_finish_flag == 0)
            dvp_start_convert();
        dvp_clear_interrupt(DVP_STS_FRAME_START);
    }

    return 0;
}

static void io_mux_init(void)
{
    /* SD card */
    fpioa_set_function(29, FUNC_SPI1_SCLK);
    fpioa_set_function(30, FUNC_SPI1_D0);
    fpioa_set_function(31, FUNC_SPI1_D1);
    fpioa_set_function(32, FUNC_GPIOHS7);

    /* Init DVP IO map and function settings */
    fpioa_set_function(11, FUNC_CMOS_RST);
    fpioa_set_function(13, FUNC_CMOS_PWDN);
    fpioa_set_function(14, FUNC_CMOS_XCLK);
    fpioa_set_function(12, FUNC_CMOS_VSYNC);
    fpioa_set_function(17, FUNC_CMOS_HREF);
    fpioa_set_function(15, FUNC_CMOS_PCLK);
    fpioa_set_function(10, FUNC_SCCB_SCLK);
    fpioa_set_function(9, FUNC_SCCB_SDA);

    /* Init SPI IO map and function settings */
    fpioa_set_function(8, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(6, FUNC_SPI0_SS3);
    fpioa_set_function(7, FUNC_SPI0_SCLK);

    fpioa_set_function(26, FUNC_GPIOHS0 + KEY_GPIONUM);
}

int main(void)
{
    FATFS sdcard_fs;

    /* Set CPU and dvp clk */
    sysctl_pll_set_freq(SYSCTL_PLL0, 800000000UL);

    io_mux_init();
    sysctl_set_spi0_dvp_data(1);

    uarths_init();

    plic_init();

    gpiohs_set_drive_mode(KEY_GPIONUM, GPIO_DM_INPUT);
    gpiohs_set_pin_edge(KEY_GPIONUM, GPIO_PE_FALLING);
    gpiohs_set_irq(KEY_GPIONUM, 2, irq_key);

    /* LCD init */
    printf("LCD init\n");
    lcd_init();
    lcd_clear(BLACK);

    /* DVP init */
    printf("DVP init\n");
    camera_init();

    dvp_set_display_addr((uint32_t)g_lcd_gram0);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_disable_auto();

    /* DVP interrupt config */
    printf("DVP interrupt config\n");
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, on_irq_dvp, NULL);
    plic_irq_enable(IRQN_DVP_INTERRUPT);

    /* enable global interrupt */
    sysctl_enable_irq();

    /* SD card init */
    if(sd_init() != 0)
    {
        lcd_draw_string(0, 0, "Fail to init SD card", RED);
        printf("Fail to init SD card\n");
        return -1;
    }
    printf("CardCapacity:%ld\n", cardinfo.CardCapacity);
    printf("CardBlockSize:%d\n", cardinfo.CardBlockSize);

    /* mount file system to SD card */
    if(f_mount(&sdcard_fs, _T("0:"), 1))
    {
        lcd_draw_string(0, 0, "Fail to mount file system", RED);
        printf("Fail to mount file system\n");
        return -1;
    }

    /* system start */
    printf("system start\n");
    g_ram_mux = 0;
    g_dvp_finish_flag = 0;
    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

    while(1)
    {
        /* ai cal finish*/
        while(g_dvp_finish_flag == 0)
            ;
        g_ram_mux ^= 0x01;
        g_dvp_finish_flag = 0;

        if(g_save_flag)
        {
            printf("Saved picture to sdcard.\n");
            rgb565tobmp((char *)(g_ram_mux ? g_lcd_gram0 : g_lcd_gram1), 320, 240, _T("0:photo.bmp"));
            g_save_flag = 0;
        }
        /* display pic*/
        lcd_draw_picture(0, 0, 320, 240, g_ram_mux ? g_lcd_gram0 : g_lcd_gram1);
    }

    return 0;
}
