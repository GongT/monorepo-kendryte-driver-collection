#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa()
{
    int ret = 0;

    ret += fpioa_set_function(36, FUNC_I2S0_IN_D0);
    ret += fpioa_set_function(37, FUNC_I2S0_WS);
    ret += fpioa_set_function(38, FUNC_I2S0_SCLK);
    ret += fpioa_set_function(33, FUNC_I2S2_OUT_D1);
    ret += fpioa_set_function(34, FUNC_I2S2_WS);
    ret += fpioa_set_function(35, FUNC_I2S2_SCLK);

    return ret;
}
