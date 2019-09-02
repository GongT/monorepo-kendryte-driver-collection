#include "fpioa-config.h"
#include <stdio.h>

__attribute__((constructor)) void initialize_kendryte_ide_hook() {
	printf("* Kendryte IDE startup before main().\n");
	ide_config_fpioa();
}
