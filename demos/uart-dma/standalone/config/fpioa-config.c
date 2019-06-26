#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa()
{
    int ret = 0;

    ret += fpioa_set_function(4, FUNC_UART3_RX);
    ret += fpioa_set_function(5, FUNC_UART3_TX);
    ret += fpioa_set_function(24, FUNC_GPIO0);

    return ret;
}
