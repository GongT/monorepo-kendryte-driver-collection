#include "stdio.h"
#include "fpioa-config.h"
static int has_already_init = 0; // back comp sdk
__attribute__((constructor)) void initialize_kendryte_ide_hook() {
	if (has_already_init) return;
	has_already_init = 1;
	printf("\n\ec\r""* Kendryte IDE startup before main().""\n");
	ide_config_fpioa();
}
