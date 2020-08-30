#ifndef __PPSAM_H__
#define __PPSAM_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "board_config.h"

#include "spi.h"
#include "sysctl.h"

#define PSRAM_BLANKS_NUM    8
#define DATALENGTH          8

//Commands for PSRAM chip
#define PSRAM_CMD_READ                 0x03
#define PSRAM_CMD_FAST_READ            0x0B
#define PSRAM_CMD_FAST_READ_DUMMY      0x3
#define PSRAM_CMD_FAST_READ_QUAD       0xEB
#define PSRAM_CMD_FAST_READ_QUAD_DUMMY 0x5
#define PSRAM_CMD_WRITE                0x02
#define PSRAM_CMD_QUAD_WRITE           0x38

#define PSRAM_CMD_ENTER_QMODE          0x35
#define PSRAM_CMD_EXIT_QMODE           0xF5

#define PSRAM_CMD_RESET_EN             0x66
#define PSRAM_CMD_RESET                0x99
#define PSRAM_CMD_SET_BURST_LEN        0xC0
#define PSRAM_CMD_DEVICE_ID            0x9F


#define PSRAM_ID_KGD_M          0xff
#define PSRAM_ID_KGD_S             8
#define PSRAM_ID_KGD            0x5d
#define PSRAM_ID_EID_M          0xff
#define PSRAM_ID_EID_S            16

// ESP-PSRAM64H MAX CLK_RATE: 133Mhz
#define PSRAM_MAX_SPI_CLK_RATE  130*1000*1000 // set max_rate: 130Mhz

// Use the [7:5](bit7~bit5) of EID to distinguish the psram size:
//
//   BIT7  |  BIT6  |  BIT5  |  SIZE(MBIT)
//   -------------------------------------
//    0    |   0    |   0    |     16
//    0    |   0    |   1    |     32
//    0    |   1    |   0    |     64
#define PSRAM_EID_SIZE_M         0x07
#define PSRAM_EID_SIZE_S            5

typedef enum {
    PSRAM_EID_SIZE_16MBITS = 0,
    PSRAM_EID_SIZE_32MBITS = 1,
    PSRAM_EID_SIZE_64MBITS = 2,
} psram_eid_size_t;

#define PSRAM_KGD(id)         (((id) >> PSRAM_ID_KGD_S) & PSRAM_ID_KGD_M)
#define PSRAM_EID(id)         (((id) >> PSRAM_ID_EID_S) & PSRAM_ID_EID_M)
#define PSRAM_SIZE_ID(id)     ((PSRAM_EID(id) >> PSRAM_EID_SIZE_S) & PSRAM_EID_SIZE_M)
#define PSRAM_IS_VALID(id)    (PSRAM_KGD(id) == PSRAM_ID_KGD)


typedef enum _psram_status_t {
    PSRAM_OK = 0,               // 
    PSRAM_ID_ERROR,             // ID ERROR or no connect
    PSRAM_NO_CONNECT,           // 
    PSRAM_WRITE_ERROR,          // 
    PSRAM_READ_ERROR,           // 
    PSRAM_BLANK_WRITE_ERROR,    // 
    PSRAM_BLANK_READ_ERROR,     // 
} psram_status_t;


// typedef struct _psram_nu_t
// {
//     uint8_t     status;             // PSRAM 状态
//     uint32_t    size;               // PSRAM 内存块大小

//     spi_frame_format_t spi_mode;    // PSRAM 当前所处于的 SPI 模式
//     spi_device_num_t spi_num;       // PSRAM 当前块的 spi num
//     spi_chip_select_t cs_num;       // PSRAM 当前块的 CS Pin

//     uint8_t     offset_id;          // PSRAM 上层指定偏移 ID
//     uint8_t     blank_id;           // PSRAM 上层指定 BLANK ID
//     uint32_t    offset_addr;        // PSRAM 上层指定偏移地址
// } psram_nu_t;

// typedef struct _psram_blank_t
// {
//     uint8_t num;            // 挂载内存块数量(PSRAM 块数)
//     uint32_t size;          // 挂载内存总大小,字节
//     psram_nu_t blanks[PSRAM_BLANKS_NUM];     // 挂载内存
// } psram_blank_t;


typedef struct _psram_config_t
{
    spi_chip_select_t chip_select_num;
    spi_device_num_t spi_num;
    spi_frame_format_t spi_ff_mode;
    uint32_t spi_clk_rate;
} psram_dev_t;



psram_status_t psram_init(void);
// psram_status_t psram_read_id(uint8_t *manuf_id, uint8_t *device_id);
// // psram_status_t psram_read_id(psram_cfg_t *cfg, uint8_t *manuf_id, uint8_t *device_id);
// psram_status_t psram_write(uint32_t addr, uint32_t *data_buf, uint32_t length);
// psram_status_t psram_read(uint32_t addr, uint32_t *data_buf, uint32_t length);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !__PPSAM_H__
