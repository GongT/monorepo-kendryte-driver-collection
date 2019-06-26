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
#include <string.h>
#include <sysctl.h>
#include <uart.h>

#define CMD_LENTH 4

#define CLOSLIGHT 0xAAAAAAAA
#define OPENLIGHT 0x55555555

#define UART_NUM UART_DEVICE_3
#define LED_IO 0

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

int on_uart_send(void *ctx)
{
    uint8_t v_uart = *((uint32_t *)ctx) + 1 + 0x30;
    uart_irq_unregister(UART_NUM, UART_SEND);
    uart_print("Send ok Uart\n");
    uart_send_data(UART_NUM, (char *)&v_uart, 1);
    return 0;
}

volatile uint32_t recv_flag = 0;
char g_cmd[4];
volatile uint8_t g_cmd_cnt = 0;
int on_uart_recv(void *ctx)
{
    char v_buf[8];
    int ret = uart_receive_data(UART_NUM, v_buf, 8);
    for(uint32_t i = 0; i < ret; i++)
    {
        if(v_buf[i] == 0x55 && (recv_flag == 0 || recv_flag == 1))
        {
            recv_flag = 1;
            continue;
        }
        else if(v_buf[i] == 0xAA && recv_flag == 1)
        {
            recv_flag = 2;
            g_cmd_cnt = 0;
            continue;
        }
        else if(recv_flag == 2 && g_cmd_cnt < CMD_LENTH)
        {
            g_cmd[g_cmd_cnt++] = v_buf[i];
            if(g_cmd_cnt >= CMD_LENTH)
            {
                release_cmd(g_cmd);
                recv_flag = 0;
            }
            continue;
        }
        else
        {
            recv_flag = 0;
        }
    }
    return 0;
}

int main()
{
    plic_init();
    sysctl_enable_irq();

    gpio_set_drive_mode(LED_IO, GPIO_DM_OUTPUT);
    gpio_set_pin(LED_IO, GPIO_PV_HIGH);

    uart_init(UART_NUM);
    uart_configure(UART_NUM, 115200, 8, UART_STOP_1, UART_PARITY_NONE);

    uart_set_receive_trigger(UART_NUM, UART_RECEIVE_FIFO_8);
    uart_irq_register(UART_NUM, UART_RECEIVE, on_uart_recv, NULL, 2);

    uart_set_send_trigger(UART_NUM, UART_SEND_FIFO_0);
    uint32_t v_uart_num = UART_NUM;
    uart_irq_register(UART_NUM, UART_SEND, on_uart_send, &v_uart_num, 2);

    char *hel = {"hello world!\n"};
    uart_send_data(UART_NUM, hel, strlen(hel));

    while(1)
        ;
}
