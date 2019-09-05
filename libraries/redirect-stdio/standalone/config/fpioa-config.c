#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa()
{
    int ret = 0;

    ret += fpioa_set_function(30, FUNC_UART2_RX);
    ret += fpioa_set_function(31, FUNC_UART2_TX);

    return ret;
}
