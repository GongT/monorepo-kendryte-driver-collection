#include "shared.h"

uint32_t spi_bus_no = 0;
uint32_t spi_chip_select = 0;

w25qxx_status_t (*w25qxx_page_program_fun)(uint32_t addr, uint8_t *data_buf, uint32_t length);
w25qxx_status_t (*w25qxx_read_fun)(uint32_t addr, uint8_t *data_buf, uint32_t length);
