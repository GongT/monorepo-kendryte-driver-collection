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
#include <plic.h>
#include <pwm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysctl.h>
#include <syslog.h>
#include <timer.h>
#include <unistd.h>
#include "pwm_play_audio.h"

#if DEMO_NUMBER == 0
#include "sounds/test_16bit_mono_wav.h"
#elif DEMO_NUMBER == 1
#include "sounds/test_16bit_wav.h"
#elif DEMO_NUMBER == 2
#include "sounds/test_24bit_wav.h"
#elif DEMO_NUMBER == 3
#include "sounds/test_8bit_wav.h"
#elif DEMO_NUMBER == 4
#include "sounds/test_wav.h"
#elif DEMO_NUMBER == 5
#include "sounds/test_welcome.h"
#else
#error "Not a valid DEMO_NUMBER value."
#endif

#define TIMER_NOR 0
#define TIMER_CHN 1
#define TIMER_PWM 0
#define TIMER_PWM_CHN 0

int main(void)
{
    printf("PWM wav test\n");
    /* Init FPIOA pin mapping for PWM*/
    fpioa_set_function(24, FUNC_TIMER0_TOGGLE1);
    /* Init Platform-Level Interrupt Controller(PLIC) */
    plic_init();
    /* Enable global interrupt for machine mode of RISC-V */
    sysctl_enable_irq();

    pwm_play_init(TIMER_NOR, TIMER_PWM);

    while(1)
    {
        pwm_play_wav(TIMER_NOR, TIMER_CHN, TIMER_PWM, TIMER_PWM_CHN, test, 0);
    }
}
