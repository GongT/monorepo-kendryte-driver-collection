#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa()
{
    int ret = 0;

    ret += fpioa_set_function(20, FUNC_GPIO0);
    ret += fpioa_set_function(21, FUNC_GPIO1);
    ret += fpioa_set_function(22, FUNC_GPIO2);
    ret += fpioa_set_function(23, FUNC_GPIO3);

    return ret;
}
