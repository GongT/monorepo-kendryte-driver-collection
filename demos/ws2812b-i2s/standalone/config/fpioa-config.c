#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa()
{
    int ret = 0;

    ret += fpioa_set_function(40, FUNC_I2S1_OUT_D3);

    return ret;
}
