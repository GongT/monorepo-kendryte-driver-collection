#include <fpioa.h>
#include "fpioa-config.h"

int ide_config_fpioa() {
int ret = 0;

ret += fpioa_set_function(24, FUNC_TIMER1_TOGGLE1);
ret += fpioa_set_function(25, FUNC_TIMER1_TOGGLE2);

return ret;
}
