#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#define  BOARD_LICHEEDAN    1

#if (BOARD_LICHEEDAN == 1)
    // PSRAM CS 
    #define PSRAM_PIN_CE_1      11
    #define PSRAM_PIN_CE_2      9
    #define PSRAM_PIN_CE_3      10
    #define PSRAM_PIN_CE_4      17

    #define PSRAM_PIN_SIO0_CLK  23

    #define PSRAM_PIN_SIO0_1    20
    #define PSRAM_PIN_SIO1_1    21
    #define PSRAM_PIN_SIO2_1    22
    #define PSRAM_PIN_SIO3_1    19

    #define PSRAM_PIN_SIO4_1    24
    #define PSRAM_PIN_SIO5_1    25
    #define PSRAM_PIN_SIO6_1    18
    #define PSRAM_PIN_SIO7_1    15
#else

#endif

#endif
