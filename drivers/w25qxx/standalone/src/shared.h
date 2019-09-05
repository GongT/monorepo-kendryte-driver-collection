#include <stdint.h>
#include "w25qxx.h"

extern uint32_t spi_bus_no;
extern uint32_t spi_chip_select;

extern w25qxx_status_t (*w25qxx_page_program_fun)(uint32_t addr, uint8_t *data_buf, uint32_t length);
extern w25qxx_status_t (*w25qxx_read_fun)(uint32_t addr, uint8_t *data_buf, uint32_t length);
