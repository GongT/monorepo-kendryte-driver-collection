#include <redirect-stdio.h>
#include <sleep.h>
#include <sysctl.h>

int main()
{
    printf("first output\n");
    msleep(1000);

    redirect_stdio();
    printf("second output\n", 0x1B);

    while(1)
    {
        printf("time: %u\r", sysctl_get_time_us());
    }
    return 1;
}
