#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa() {
int ret = 0;

ret += fpioa_set_function(6, FUNC_SPI0_SS3);
ret += fpioa_set_function(7, FUNC_SPI0_SCLK);
ret += fpioa_set_function(8, FUNC_GPIOHS2);
ret += fpioa_set_function(11, FUNC_CMOS_RST);
ret += fpioa_set_function(13, FUNC_CMOS_PWDN);
ret += fpioa_set_function(14, FUNC_CMOS_XCLK);
ret += fpioa_set_function(12, FUNC_CMOS_VSYNC);
ret += fpioa_set_function(17, FUNC_CMOS_HREF);
ret += fpioa_set_function(15, FUNC_CMOS_PCLK);
ret += fpioa_set_function(10, FUNC_SCCB_SCLK);
ret += fpioa_set_function(9, FUNC_SCCB_SDA);

return ret;
}
