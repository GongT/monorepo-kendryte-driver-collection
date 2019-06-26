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

#define RECV_LENTH 4

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

int main()
{
    plic_init();
    sysctl_enable_irq();

    gpio_set_drive_mode(LED_IO, GPIO_DM_OUTPUT);
    gpio_set_pin(LED_IO, GPIO_PV_HIGH);

    uart_init(UART_NUM);
    uart_configure(UART_NUM, 115200, 8, UART_STOP_1, UART_PARITY_NONE);

    char *hel = {"hello world!\n"};
    uart_send_data(UART_NUM, hel, strlen(hel));

    char recv = 0;
    int rec_flag = 0;
    char cmd[8];
    int i = 0;
    while(1)
    {
        while(uart_receive_data(UART_NUM, &recv, 1))
        {
            switch(rec_flag)
            {
                case 0:
                    recv == 0x55 ? (rec_flag = 1) : rec_flag;
                    break;
                case 1:
                    recv == 0xAA ? (rec_flag = 2) : (rec_flag = 0);
                    break;
                case 2:
                    cmd[i++] = recv;
                    if(i >= RECV_LENTH)
                    {
                        i = 0;
                        release_cmd(cmd);
                        rec_flag = 0;
                    }
                    break;
            }
        }
    }
    while(1)
        ;
}
