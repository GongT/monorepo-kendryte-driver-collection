#include <fpioa.h>
#include <gpio.h>
#include <pwm.h>
#include <sleep.h>
#include <stdio.h>

#define TIMER_PWM 1
#define TIMER_PWM_CHN 0

int main(void) {
  pwm_init(TIMER_PWM);
  pwm_set_enable(TIMER_PWM, TIMER_PWM_CHN, 1);

  gpio_init();
  gpio_set_drive_mode(0, GPIO_DM_INPUT_PULL_DOWN);
  gpio_set_drive_mode(1, GPIO_DM_INPUT_PULL_DOWN);
  gpio_set_drive_mode(2, GPIO_DM_INPUT_PULL_DOWN);
  gpio_set_drive_mode(3, GPIO_DM_INPUT_PULL_DOWN);

  gpio_pin_value_t a, b, c, d;
  int lastValue = 5;
  int currentValue;
  pwm_set_frequency(TIMER_PWM, TIMER_PWM_CHN, 200000, (double)lastValue / 9);

  while (1) {
    a = gpio_get_pin(0);
    b = gpio_get_pin(1);
    c = gpio_get_pin(2);
    d = gpio_get_pin(3);

    currentValue = a * 8 + b * 4 + c * 2 + d;
    if(currentValue>9){
      continue;
    }

    if (currentValue == lastValue) {
      msleep(10);
      continue;
    }

    printf("%f\n", 1.0*currentValue / 9);
    pwm_set_frequency(TIMER_PWM, TIMER_PWM_CHN, 200000, 1.0*currentValue / 9);

    lastValue = currentValue;
    msleep(10);
  }
}
