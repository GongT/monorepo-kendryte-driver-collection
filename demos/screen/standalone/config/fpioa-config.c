#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa() {
int ret = 0;

ret += fpioa_set_function(6, FUNC_SPI0_SS3);
ret += fpioa_set_function(7, FUNC_SPI0_SCLK);
ret += fpioa_set_function(8, FUNC_GPIOHS2);

return ret;
}
