/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:28:18
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-06 01:47:58
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\src\ads1299.c
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#include "ads1299.h"
#include "nrfx_spim.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "app_error.h"
#include <string.h>

#define NRFX_SPIM_SCK_PIN 3
#define NRFX_SPIM_MOSI_PIN 4
#define NRFX_SPIM_MISO_PIN 28
#define NRFX_SPIM_SS_PIN 29

#define ADS_PWR_PIN NRF_GPIO_PIN_MAP(1, 1)
#define ADS_RESET_PIN NRF_GPIO_PIN_MAP(1, 2)
#define ADS_START_PIN NRF_GPIO_PIN_MAP(1, 3)
#define ADS_RDY_PIN NRF_GPIO_PIN_MAP(1, 3)

#define SPI_INSTANCE 3                                           /**< SPI instance index. */
static const nrfx_spim_t spi = NRFX_SPIM_INSTANCE(SPI_INSTANCE); /**< SPI instance. */
static volatile bool spi_xfer_done;                              /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t m_tx_buf[32] = {0};  /**< TX buffer. */
static uint8_t m_rx_buf[32] = {0};  /**< RX buffer. */
static uint8_t m_length = 32; /**< Transfer length. */

/**
 * @brief SPI user event handler.
 * @param event
 */
void spim_event_handler(nrfx_spim_evt_t const *p_event,
                        void *p_context)
{
    spi_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_INFO(" Received:");
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
}

static int ads_reg_xfer(uint8_t command, uint8_t value)
{
		nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TRX(m_tx_buf, 3, m_rx_buf, 3);

    spi_xfer_done = false;
    m_tx_buf[0] = command;
    m_tx_buf[1] = 0x00;
    if ((command & 0x20) == 0x20)
    {
        m_tx_buf[2] = 0x00;
    }
    else if ((command & 0x40) == 0x40)
    {
        m_tx_buf[2] = value;
    }
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer_desc, 0));
    while (!spi_xfer_done)
    {
    }
    spi_xfer_done = false;
    return 0;
}

static int ads_xfer(uint8_t value)
{
    spi_xfer_done = false;
    m_tx_buf[0] = value;
    nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TX(m_tx_buf, 1);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer_desc, 0));
    while (!spi_xfer_done)
    {
    }
    spi_xfer_done = false;
    return 0;
}

static int ads_read_sample(void)
{
    spi_xfer_done = false;
    memset(m_rx_buf, 0, 30);
    nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TX(m_rx_buf, 27);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer_desc, 0));
    while (!spi_xfer_done)
    {
    }
    spi_xfer_done = false;
    return 0;
}
static void spi_init(void)
{

    nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG;
    spi_config.frequency = NRF_SPIM_FREQ_8M;
    spi_config.ss_pin = NRFX_SPIM_SS_PIN;
    spi_config.miso_pin = NRFX_SPIM_MISO_PIN;
    spi_config.mosi_pin = NRFX_SPIM_MOSI_PIN;
    spi_config.sck_pin = NRFX_SPIM_SCK_PIN;
    spi_config.use_hw_ss = true;
    spi_config.mode = NRF_SPIM_MODE_1;
    spi_config.ss_active_high = false;
    APP_ERROR_CHECK(nrfx_spim_init(&spi, &spi_config, spim_event_handler, NULL));
}
static void rdy_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("Test");
    ads_read_sample();
}
static void gpio_init(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(ADS_RDY_PIN, &in_config, rdy_handler);
    APP_ERROR_CHECK(err_code);

    // nrf_drv_gpiote_in_event_enable(ADS_RDY_PIN, true);

    nrf_gpio_cfg_output(ADS_PWR_PIN);
    nrf_gpio_pin_clear(ADS_PWR_PIN);

    nrf_gpio_cfg_output(ADS_RESET_PIN);
    nrf_gpio_pin_clear(ADS_RESET_PIN);

    nrf_gpio_cfg_output(ADS_START_PIN);
    nrf_gpio_pin_clear(ADS_START_PIN);
}

static void ads_power_on(void)
{
    nrf_gpio_pin_set(ADS_PWR_PIN);
}

static void ads_reset(void)
{
    nrf_gpio_pin_set(ADS_RESET_PIN);
    nrf_delay_ms(50);
    nrf_gpio_pin_clear(ADS_RESET_PIN);
    nrf_delay_ms(10);
    nrf_gpio_pin_set(ADS_RESET_PIN);
}

static void ads_clear_reset(void)
{
    nrf_gpio_pin_set(ADS_RESET_PIN);
}

void ads_1299_start(void)
{
    nrf_gpio_pin_set(ADS_START_PIN);
}
int ads_1299_init(void)
{
    spi_init();
    gpio_init();
    ads_power_on();
    ads_clear_reset();
    nrf_delay_ms(150);
		ads_xfer(SDATAC);
    nrf_delay_ms(10);
    ads_reg_xfer(WREG | CONFIG3, 0xC0);
    nrf_delay_ms(10);
    ads_reg_xfer(WREG | CONFIG1, 0xC6);
    nrf_delay_ms(10);
    ads_xfer(RDATAC);
    nrf_delay_ms(10);
    return 0;
}

int ads_config_freq()
{
}
uint16_t ads_1299_read(void)
{

    return 0;
}
