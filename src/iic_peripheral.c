/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 15:36:50
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-01 23:00:03
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\src\iic_peripheral.c
 * @Description: 
 * 
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved. 
 */


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_drv_twi.h"
#include "app_error.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "user_log.h"

#define MASTER_IIC_INST     0       
#define TWI_SCL_M           3       //!< Master SCL pin.
#define TWI_SDA_M           4      //!< Master SDA pin.

static const nrf_drv_twi_t m_iic_master = NRF_DRV_TWI_INSTANCE(MASTER_IIC_INST);
static bool iic_init_status = false;

ret_code_t iic_write(uint8_t addr, uint8_t const * pdata, uint8_t size)
{
    ret_code_t ret;
    ret = nrf_drv_twi_tx(&m_iic_master, addr, pdata, size, true);
    return ret;
}

ret_code_t iic_read(uint8_t addr, uint8_t * pdata, uint8_t size)
{
    ret_code_t ret;
    ret = nrf_drv_twi_rx(&m_iic_master, addr, pdata, size);
    return ret;
}
/**
 * @brief iic events handler.
 */
void iic_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    NRF_LOG_INFO("iic_handler event type = %d",p_event->type);
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            break;
        default:
            break;
    }
}

/**
 * @brief Initialize the master TWI.
 *
 * Function used to initialize the master TWI interface that would communicate with simulated EEPROM.
 *
 * @return NRF_SUCCESS or the reason of failure.
 */
ret_code_t iic_master_init(void)
{
    if(iic_init_status)
    {
        return NRF_SUCCESS;
    }

    ret_code_t ret;
    const nrf_drv_twi_config_t config =
    {
       .scl                = TWI_SCL_M,
       .sda                = TWI_SDA_M,
       .frequency          = NRF_DRV_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    ret = nrf_drv_twi_init(&m_iic_master, &config, NULL, NULL);

    if (NRF_SUCCESS == ret)
    {
        nrf_drv_twi_enable(&m_iic_master);
    }
    iic_init_status = true;

    return ret;
}


