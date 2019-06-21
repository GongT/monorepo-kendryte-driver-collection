#include <gpio.h>
#include <sleep.h>

int main(void)
{
    gpio_init();
    gpio_pin_value_t a, b, c, d;
    while(1)
    {
        a = gpio_get_pin(0);
        b = gpio_get_pin(1);
        c = gpio_get_pin(2);
        d = gpio_get_pin(3);
        printk("Current value is: %d     \r", a * 8 + b * 4 + c * 2 + d);
        sleep(1);
    }
}
