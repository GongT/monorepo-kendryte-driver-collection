#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa() {
int ret = 0;

ret += fpioa_set_function(30, FUNC_SPI0_D0);
ret += fpioa_set_function(31, FUNC_SPI0_D1);
ret += fpioa_set_function(29, FUNC_SPI0_SCLK);
ret += fpioa_set_function(32, FUNC_GPIOHS7);
ret += fpioa_set_function(24, FUNC_SPI0_SS3);
ret += fpioa_set_function(33, FUNC_I2S0_OUT_D0);
ret += fpioa_set_function(35, FUNC_I2S0_SCLK);
ret += fpioa_set_function(34, FUNC_I2S0_WS);

return ret;
}
