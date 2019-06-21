#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa() {
int ret = 0;

ret += fpioa_set_function(26, FUNC_GPIOHS0);
ret += fpioa_set_function(25, FUNC_GPIOHS1);

return ret;
}
