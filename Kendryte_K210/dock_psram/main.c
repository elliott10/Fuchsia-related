#include <stdio.h>
#include "sysctl.h"
#include "uarths.h"
#include "syslog.h"


#include "board_config.h"
#include "psram.h"


static const char *TAG = "main";


int main(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, 800000000);
    uarths_init();
    
    printk(LOG_COLOR_W "-------------------------------\r\n");
    printk(LOG_COLOR_W "Sipeed@QinYUN575\r\n");
    printk(LOG_COLOR_W "%s:" "Compile@ %s %s\r\n", __FUNCTION__, __DATE__, __TIME__);
    printk(LOG_COLOR_W "-------------------------------\r\n");
    
    psram_init();
    
    LOGI(TAG, "----------END-----------");
    while (1);
    return 0;
}