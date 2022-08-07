/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:28:18
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-08 04:00:55
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\src\max30205.c
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#include "max30205.h"
#include "iic_peripheral.h"
#include "app_fifo.h"
#include "customer_ble.h"
#define MAX30250_ADDR (0x90 >> 1)
#define TEMP_REGISTER (0x00)
#define CONFIG_REGISTER (0x01)
#define CONFIG_VALUE (0x40)

#define TEMP_DATA_BUFF_SIZE 1024
static app_fifo_t m_temp_data_fifo;

static uint8_t p_data_read[2] = {TEMP_REGISTER, 0x00};

static void temp_data_fifo_init(void)
{
    ret_code_t err_code;
    static uint8_t temp_data_buff[TEMP_DATA_BUFF_SIZE]; // Buffer for NFC TX FIFO instance

    err_code = app_fifo_init(&m_temp_data_fifo, temp_data_buff, TEMP_DATA_BUFF_SIZE);
    APP_ERROR_CHECK(err_code);
}

bool max_30205_init(void)
{
    ret_code_t ret_code;

    temp_data_fifo_init();
    uint8_t p_data[2] = {CONFIG_REGISTER, CONFIG_VALUE};
    if (iic_master_init())
    {
        NRF_LOG_INFO("iic_master_init fail");
        return false;
    }

    ret_code = iic_write(MAX30250_ADDR, p_data, sizeof(p_data));
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("max_30205 config fail");
        return false;
    }

    NRF_LOG_INFO("max_30205_init success");
    return true;
}

uint16_t max_30205_read(void)
{
    ret_code_t ret_code;

    p_data_read[0] = TEMP_REGISTER;
    ret_code = iic_write(MAX30250_ADDR, p_data_read, 1);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("iic_write err = %d", ret_code);
        return false;
    }

    ret_code = iic_read(MAX30250_ADDR, p_data_read, sizeof(p_data_read));
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_ERROR("iic_read err = %d", ret_code);
        return false;
    }
    if (trans_status)
    {
        app_fifo_write(&m_temp_data_fifo, p_data_read, 2);
    }
    else
    {
        app_fifo_flush(&m_temp_data_fifo);
    }
    //    NRF_LOG_INFO("temp hex data = %d", ((p_data_read[0] << 8) + p_data_read[1]));
    return 0;
}
