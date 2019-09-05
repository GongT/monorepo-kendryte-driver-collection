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
#include <gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysctl.h>
#include <uart.h>
#include <unistd.h>

#define CMD_LENTH 4

#define CLOSLIGHT 0xAAAAAAAA
#define OPENLIGHT 0x55555555

#define UART_NUM UART_DEVICE_3
#define LED_IO 0

uint32_t recv_buf[48];
#define RECV_DMA_LENTH 6

volatile uint32_t recv_flag = 0;
char g_cmd[4];
volatile uint8_t g_cmd_cnt = 0;

void uart_print(const char const *str)
{
    uart_send_data(UART_NUM, str, strlen(str));
}

int release_cmd(char *cmd)
{
    unsigned char hex[256] = "";
    for(unsigned int i = 0; cmd[i] != '\0'; i++)
    {
        sprintf((char *)(hex + i * 2), "%02X", cmd[i]);
    }
    unsigned char print[256] = "";
    sprintf(print, "release_cmd(%s):\n", hex);
    uart_print(print);

    switch(*((int *)cmd))
    {
        case CLOSLIGHT:
            uart_print("CLOSLIGHT.\n");
            gpio_set_pin(LED_IO, GPIO_PV_HIGH);
            break;
        case OPENLIGHT:
            uart_print("OPENLIGHT.\n");
            gpio_set_pin(LED_IO, GPIO_PV_LOW);
            break;
        default:
            uart_print("Unknown cmd.\n");
    }
    return 0;
}

volatile uint32_t g_uart_send_flag = 0;

int uart_send_done(void *ctx)
{
    g_uart_send_flag = 1;
    return 0;
}

int uart_recv_done(void *ctx)
{
    uint32_t *v_dest = ((uint32_t *)ctx) + RECV_DMA_LENTH;
    if(v_dest >= recv_buf + 48)
        v_dest = recv_buf;

    uart_data_t data = (uart_data_t){
        .rx_channel = DMAC_CHANNEL1,
        .rx_buf = v_dest,
        .rx_len = RECV_DMA_LENTH,
        .transfer_mode = UART_RECEIVE,
    };

    plic_interrupt_t irq = (plic_interrupt_t){
        .callback = uart_recv_done,
        .ctx = v_dest,
        .priority = 2,
    };

    uart_handle_data_dma(UART_NUM, data, &irq);
    uint32_t *v_buf = (uint32_t *)ctx;
    for(uint32_t i = 0; i < RECV_DMA_LENTH; i++)
    {
        if(v_buf[i] == 0x55 && (recv_flag == 0 || recv_flag == 1))
        {
            recv_flag = 1;
            continue;
        } else if(v_buf[i] == 0xAA && recv_flag == 1)
        {
            recv_flag = 2;
            g_cmd_cnt = 0;
            continue;
        } else if(recv_flag == 2 && g_cmd_cnt < CMD_LENTH)
        {
            g_cmd[g_cmd_cnt++] = v_buf[i];
            if(g_cmd_cnt >= CMD_LENTH)
            {
                release_cmd(g_cmd);
                recv_flag = 0;
            }
            continue;
        } else
        {
            recv_flag = 0;
        }
    }

    return 0;
}

int main(void)
{
    dmac_init();
    plic_init();
    sysctl_enable_irq();

    gpio_set_drive_mode(LED_IO, GPIO_DM_OUTPUT);
    gpio_set_pin(LED_IO, GPIO_PV_HIGH);

    uart_init(UART_NUM);
    uart_configure(UART_NUM, 115200, 8, UART_STOP_1, UART_PARITY_NONE);

    uint8_t *hel = {"hello!\n"};

    uint32_t *v_tx_buf = malloc(sizeof(hel) * sizeof(uint32_t));
    for(uint32_t i = 0; i < strlen(hel); i++)
    {
        v_tx_buf[i] = hel[i];
    }

    uart_data_t data = (uart_data_t){
        .tx_channel = DMAC_CHANNEL0,
        .tx_buf = v_tx_buf,
        .tx_len = strlen(hel),
        .transfer_mode = UART_SEND,
    };

    plic_interrupt_t irq = (plic_interrupt_t){
        .callback = uart_send_done,
        .ctx = NULL,
        .priority = 1,
    };

    uart_handle_data_dma(UART_NUM, data, &irq);

    uart_data_t v_rx_data = (uart_data_t){
        .rx_channel = DMAC_CHANNEL1,
        .rx_buf = recv_buf,
        .rx_len = RECV_DMA_LENTH,
        .transfer_mode = UART_RECEIVE,
    };

    plic_interrupt_t v_rx_irq = (plic_interrupt_t){
        .callback = uart_recv_done,
        .ctx = recv_buf,
        .priority = 2,
    };
    uart_handle_data_dma(UART_NUM, v_rx_data, &v_rx_irq);
    while(1)
    {
        sleep(1);
        uart_handle_data_dma(UART_NUM, data, &irq);
        g_uart_send_flag = 1;
    }
}
