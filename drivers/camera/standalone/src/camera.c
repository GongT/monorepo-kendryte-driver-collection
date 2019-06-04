#include <dvp.h>
#include <fpioa.h>
#include <gpiohs.h>
#include <sysctl.h>
#include "ov2640.h"
#include "ov5640.h"


static void io_set_power(void)
{
#if BOARD_LICHEEDAN
        /* Set dvp and spi pin to 1.8V */
        sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
        sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
#else
        /* Set dvp and spi pin to 1.8V */
        sysctl_set_power_mode(SYSCTL_POWER_BANK0, SYSCTL_POWER_V18);
        sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
        sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V18);
#endif
}

void camera_init()
{
    sysctl_set_spi0_dvp_data(1);
    gpiohs_set_drive_mode(8, GPIO_DM_INPUT);

  io_set_power();

#if OV5640
    dvp_init(16);
    dvp_set_xclk_rate(12000000);
    dvp_enable_burst();
    dvp_set_output_enable(0, 1);
    dvp_set_output_enable(1, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    dvp_set_image_size(320, 240);
    ov5640_init();
#else
    dvp_init(8);
    dvp_set_xclk_rate(24000000);
    dvp_enable_burst();
    dvp_set_output_enable(0, 1);
    dvp_set_output_enable(1, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    dvp_set_image_size(320, 240);
    ov2640_init();
#endif
}
