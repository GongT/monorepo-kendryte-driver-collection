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
#include <gpiohs.h>
#include <stdio.h>
#include <sysctl.h>
#include <unistd.h>

uint32_t irq_flag;

#define GPIO_LED 1
#define GPIO_KEY 0

uint32_t g_count;

int irq_gpiohs2(void *ctx)
{
    irq_flag = gpiohs_get_pin(0);

    printf("IRQ The PIN is %d\n", irq_flag);

    uint32_t *tmp = (uint32_t *)(ctx);
    printf("count is %d\n", (*tmp)++);

    if(!irq_flag)
        gpiohs_set_pin(GPIO_LED, GPIO_PV_LOW);
    else
        gpiohs_set_pin(GPIO_LED, GPIO_PV_HIGH);
    return 0;
}

int main(void)
{
    sleep(2);
    plic_init();
    sysctl_enable_irq();

    fpioa_set_function(26, FUNC_GPIOHS0);
    fpioa_set_function(25, FUNC_GPIOHS1);

    gpiohs_set_drive_mode(GPIO_LED, GPIO_DM_OUTPUT);
    gpio_pin_value_t value = GPIO_PV_HIGH;
    gpiohs_set_pin(GPIO_LED, value);

    gpiohs_set_drive_mode(GPIO_KEY, GPIO_DM_INPUT_PULL_UP);
    gpiohs_set_pin_edge(GPIO_KEY, GPIO_PE_BOTH);

    gpiohs_irq_register(GPIO_KEY, 1, irq_gpiohs2, &g_count);

    while(1)
        ;
}
