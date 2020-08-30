#include <stdio.h>
#include "fpioa.h"
#include "sysctl.h"

#include "spi.h"
#include "syslog.h"

#include "board_config.h"
#include "psram.h"

static const char *TAG = "psam";


#define SPI_SCLK_RATE 10*1000*1000 // 10 Mhz
#define SPI_INDEX SPI_DEVICE_0

psram_dev_t dev;

void io_mux_init(void)
{
    LOGI(TAG, "%s:" "", __FUNCTION__);
    fpioa_set_function(PSRAM_PIN_CE_1,  FUNC_SPI0_SS0);  // CS
    fpioa_set_function(PSRAM_PIN_CE_2,  FUNC_SPI0_SS1);
    fpioa_set_function(PSRAM_PIN_CE_3,  FUNC_SPI0_SS2);
    fpioa_set_function(PSRAM_PIN_CE_4,  FUNC_SPI0_SS3);
    
    fpioa_set_function(PSRAM_PIN_SIO0_CLK,  FUNC_SPI0_SCLK); // SCLK
    
    fpioa_set_function(PSRAM_PIN_SIO0_1,    FUNC_SPI0_D0);   // MOSI
    fpioa_set_function(PSRAM_PIN_SIO1_1,    FUNC_SPI0_D1);   // MISO
    fpioa_set_function(PSRAM_PIN_SIO2_1,    FUNC_SPI0_D2);
    fpioa_set_function(PSRAM_PIN_SIO3_1,    FUNC_SPI0_D3);

    fpioa_set_function(PSRAM_PIN_SIO4_1,    FUNC_SPI0_D4);
    fpioa_set_function(PSRAM_PIN_SIO5_1,    FUNC_SPI0_D5);
    fpioa_set_function(PSRAM_PIN_SIO6_1,    FUNC_SPI0_D6);
    fpioa_set_function(PSRAM_PIN_SIO7_1,    FUNC_SPI0_D7);
}

psram_status_t psram_spi_init(spi_device_num_t spi_num, uint32_t spi_clk_rate)
{
    LOGI(TAG, "%s:" "SPI:%d, clk_rate:%d", __FUNCTION__, spi_num, spi_clk_rate);
    dev.spi_num = spi_num;
    dev.spi_clk_rate = spi_clk_rate;
    
    spi_init(dev.spi_num, SPI_WORK_MODE_0, dev.spi_ff_mode, DATALENGTH, 0);
    spi_set_clk_rate(dev.spi_num, dev.spi_clk_rate);
    return PSRAM_OK;
}
// ----------------------------------
static psram_status_t psram_receive_data(spi_chip_select_t cs_num, uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff, uint32_t rx_len)
{
    dev.chip_select_num = cs_num;
    spi_init(dev.spi_num, SPI_WORK_MODE_0, dev.spi_ff_mode, DATALENGTH, 0);
    spi_receive_data_standard(dev.spi_num, dev.chip_select_num, cmd_buff, cmd_len, rx_buff, rx_len);
    return PSRAM_OK;
}

static psram_status_t psram_send_data(spi_chip_select_t cs_num, uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
{
    dev.chip_select_num = cs_num;
    spi_init(dev.spi_num, SPI_WORK_MODE_0, dev.spi_ff_mode, DATALENGTH, 0);
    spi_send_data_standard(dev.spi_num, dev.chip_select_num, cmd_buff, cmd_len, tx_buff, tx_len);
    return PSRAM_OK;
}
// ----------------------------------
static psram_status_t psram_receive_multiple(spi_chip_select_t cs_num, uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff, uint32_t rx_len)
{
    dev.chip_select_num = cs_num;
    spi_init(dev.spi_num, SPI_WORK_MODE_0, dev.spi_ff_mode, DATALENGTH, 0);
    spi_receive_data_multiple(dev.spi_num, dev.chip_select_num, cmd_buff, cmd_len, rx_buff, rx_len);
    return PSRAM_OK;
}

static psram_status_t psram_send_multiple(spi_chip_select_t cs_num, uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
{
    dev.chip_select_num = cs_num;
    spi_init(dev.spi_num, SPI_WORK_MODE_0, dev.spi_ff_mode, DATALENGTH, 0);
    spi_send_data_multiple(dev.spi_num, dev.chip_select_num, cmd_buff, cmd_len, tx_buff, tx_len);
    return PSRAM_OK;
}

// ----------------------------------
psram_status_t psram_write_enable(spi_chip_select_t cs_num)
{
    uint8_t cmd[1] = {PSRAM_CMD_WRITE};
    psram_send_data(cs_num, cmd, 1, NULL, 0);
    return PSRAM_OK;
}

psram_status_t psram_enbale_quad_mode(spi_chip_select_t cs_num)
{
    dev.spi_ff_mode = SPI_FF_STANDARD;
    psram_write_enable(cs_num);
    uint8_t cmd[1] = {PSRAM_CMD_ENTER_QMODE};
    psram_send_data(cs_num, cmd, 1, NULL, 0);
    dev.spi_ff_mode = SPI_FF_QUAD;
    return PSRAM_OK;
}

psram_status_t psram_write_multiple_enable(spi_chip_select_t cs_num)
{
    uint8_t cmd[1] = {PSRAM_CMD_QUAD_WRITE};
    psram_send_multiple(cs_num, cmd, 1, NULL, 0);
    return PSRAM_OK;
}

psram_status_t psram_exit_quad_mode(spi_chip_select_t cs_num)
{
    dev.spi_ff_mode = SPI_FF_QUAD;
    psram_write_multiple_enable(cs_num);
    uint8_t cmd[1] = {PSRAM_CMD_EXIT_QMODE};
    psram_send_multiple(cs_num, cmd, 1, NULL, 0);
    dev.spi_ff_mode = SPI_FF_STANDARD;
    return PSRAM_OK;
}

// ----------------------------------
psram_status_t psram_spi_read_id(spi_chip_select_t cs_num, uint8_t *kgd_id)
{
    uint8_t cmd[4] = {PSRAM_CMD_DEVICE_ID, 0x00, 0x00, 0x00};
    uint8_t data[2] = {0};

    psram_exit_quad_mode(cs_num);
    psram_receive_data(cs_num, cmd, 4, data, 2);

    uint8_t manuf_id = data[0];
    *kgd_id = data[1];
    LOGD(TAG, "manuf_id:0x%02x, KGD:0x%02x", manuf_id, *kgd_id);
    if ((manuf_id != 0x0d) || (*kgd_id != 0x5d))
    {
        LOGE(TAG, "manuf_id:0x%02x, KGD:0x%02x", manuf_id, *kgd_id);
        return PSRAM_ID_ERROR;
    }
    return PSRAM_OK;
}


psram_status_t psram_init(void)
{

    LOGI(TAG, "%s:" "", __FUNCTION__);

    io_mux_init();
    psram_spi_init(SPI_INDEX, SPI_SCLK_RATE);
    uint8_t kgd_id;

    psram_spi_read_id(SPI_CHIP_SELECT_0, &kgd_id);
    psram_enbale_quad_mode(SPI_CHIP_SELECT_0);

    psram_spi_read_id(SPI_CHIP_SELECT_1, &kgd_id);
    psram_enbale_quad_mode(SPI_CHIP_SELECT_1);
    
    psram_spi_read_id(SPI_CHIP_SELECT_2, &kgd_id);
    // psram_enbale_quad_mode(SPI_CHIP_SELECT_2);
    
    psram_spi_read_id(SPI_CHIP_SELECT_3, &kgd_id);
    // psram_enbale_quad_mode(SPI_CHIP_SELECT_3);

    return PSRAM_OK;
}

