/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:28:18
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-08 06:52:01
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\src\bmi270.c
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#include "bmi270.h"
#include "iic_peripheral.h"
#include "nrf_delay.h"
#include "bmi270_config_file.h"
#include "app_fifo.h"
#include "customer_ble.h"

#define BMI270_ADDR (0x68)
#define BMI270_WR_LEN (256)
#define BMI270_CONFIG_FILE_RETRIES (15)
#define BMI270_CONFIG_FILE_POLL_PERIOD_US (10000)
#define BMI270_INTER_WRITE_DELAY_US (1000)

#define IMU_DATA_BUFF_SIZE 8192
static app_fifo_t m_imu_data_fifo;

static int8_t write_config_file(void)
{
    int8_t ret = 0;
    uint16_t index = 0;
    uint16_t remain_length = 0;
    uint8_t addr_array[3] = {BMI270_REG_INIT_ADDR_0};
    uint8_t data_array[BMI270_WR_LEN + 1] = {BMI270_REG_INIT_DATA};
    /* Disable loading of the configuration */
    for (index = 0; index < sizeof(bmi270_config_file);
         index += BMI270_WR_LEN)
    {
        /* Store 0 to 3 bits of address in first byte */
        addr_array[1] = (uint8_t)((index / 2) & 0x0F);

        /* Store 4 to 11 bits of address in the second byte */
        addr_array[2] = (uint8_t)((index / 2) >> 4);

        ret = iic_write(BMI270_ADDR, addr_array, 3);
        if (NRF_SUCCESS == ret)
        {
            nrf_delay_us(BMI270_INTER_WRITE_DELAY_US);
            ret = iic_write(BMI270_ADDR, addr_array, 3);
            if (NRF_SUCCESS == ret)
            {
                nrf_delay_us(BMI270_INTER_WRITE_DELAY_US);
                remain_length = sizeof(bmi270_config_file) - index;
                memset(&data_array[1], 0, BMI270_WR_LEN);
                if (remain_length >= BMI270_WR_LEN)
                {
                    memcpy(&data_array[1], (bmi270_config_file + index), BMI270_ADDR);
                    remain_length = BMI270_WR_LEN;
                }
                else
                {
                    memcpy(&data_array[1], (bmi270_config_file + index), remain_length);
                }

                ret = iic_write(BMI270_ADDR, data_array, remain_length + 1);
                if (NRF_SUCCESS == ret)
                {
                    nrf_delay_us(BMI270_INTER_WRITE_DELAY_US);
                }
            }
        }
    }
    NRF_LOG_INFO("Write config file ret = %d", ret);
    return ret;
}

static bool bmi270_setup_config(void)
{
    ret_code_t ret_code;

    uint8_t p_data[2];

    nrf_delay_us(BMI270_POWER_ON_TIME);
    if (iic_master_init())
    {
        NRF_LOG_INFO("iic_master_init fail");
        return false;
    }

    // check chip id
    p_data[0] = BMI270_REG_CHIP_ID;
    ret_code = iic_write(BMI270_ADDR, p_data, 1);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 write read point fail");
        return false;
    }
    ret_code = iic_read(BMI270_ADDR, p_data, 1);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 chip id read fail");
        return false;
    }
    if (p_data[0] != BMI270_CHIP_ID)
    {
        NRF_LOG_ERROR("Unexpected chip id (%x). Expected (%x)", p_data[0], BMI270_CHIP_ID);
        return false;
    }
    NRF_LOG_INFO("bmi270 chip id =%d", p_data[0]);

    p_data[0] = BMI270_REG_CMD;
    p_data[1] = BMI270_CMD_SOFT_RESET;
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 soft reset fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 soft reset success");
    nrf_delay_us(BMI270_SOFT_RESET_TIME);

    // config pwr register
    p_data[0] = BMI270_REG_PWR_CONF;
    p_data[1] = 0;
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 soft reset fail");
        return false;
    }
    p_data[0] = BMI270_REG_PWR_CONF;
    p_data[1] = BMI270_SET_BITS_POS_0(p_data[1],
                                      BMI270_PWR_CONF_ADV_PWR_SAVE,
                                      BMI270_PWR_CONF_ADV_PWR_SAVE_DIS);
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 disable pwr fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 disable pwr success");

    // wait for config finish
    nrf_delay_ms(2);

    p_data[0] = BMI270_REG_INIT_CTRL;
    p_data[1] = BMI270_PREPARE_CONFIG_LOAD;
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 prepare config load fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 prepare config load success");

    ret_code = write_config_file();
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 write config file fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 write config file success");

    p_data[0] = BMI270_REG_INIT_CTRL;
    p_data[1] = BMI270_COMPLETE_CONFIG_LOAD;
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 complete config load fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 complete config load success");

    nrf_delay_ms(200);
    p_data[0] = BMI270_REG_INTERNAL_STATUS;
    ret_code = iic_write(BMI270_ADDR, p_data, 1);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 write init status fail");
        return false;
    }
    for (uint8_t i = 0; i <= BMI270_CONFIG_FILE_RETRIES; i++)
    {
        ret_code = iic_read(BMI270_ADDR, p_data, 1);
        if (ret_code != 0)
        {
            NRF_LOG_INFO("bmi270 read init status fail");
            return false;
        }

        p_data[0] &= BMI270_INST_MESSAGE_MSK;
        if (p_data[0] == BMI270_INST_MESSAGE_INIT_OK)
        {
            break;
        }

        nrf_delay_us(BMI270_CONFIG_FILE_POLL_PERIOD_US);
    }

    NRF_LOG_INFO("bmi270 success");
    return true;
}
static bool bmi270_set_performance_mode(void)
{
    ret_code_t ret_code;

    uint8_t p_data[2];
    p_data[0] = BMI270_REG_PWR_CTRL;
    // p_data[1] = (BMI270_PWR_CTRL_AUX_EN | BMI270_PWR_CTRL_ACC_EN | BMI270_PWR_CTRL_TEMP_EN) & (BMI270_PWR_CTRL_MSK);
    p_data[1] = 0x0E;
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 pwr ctrl fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 pwr ctrl success");

    p_data[0] = BMI270_REG_ACC_CONF;
    p_data[1] = 0xA8;
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 acc config fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 acc config success");

    p_data[0] = BMI270_REG_GYR_CONF;
    p_data[1] = 0xE9;
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 gyr config fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 gyr config success");

    p_data[0] = BMI270_REG_PWR_CONF;
    p_data[1] = 0x02;
    ret_code = iic_write(BMI270_ADDR, p_data, 2);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 pwr config fail");
        return false;
    }
    NRF_LOG_INFO("bmi270 pwr config success");

    // p_data[0] = BMI270_REG_ACC_X_LSB;
    // ret_code = iic_write(BMI270_ADDR, p_data, 1);
    // if (NRF_SUCCESS != ret_code)
    // {
    //     NRF_LOG_INFO("bmi270 set read pointer fail");
    //     return false;
    // }
    // NRF_LOG_INFO("bmi270 set read pointer success");
    return true;
}
bool get_imu_fifo_data(uint8_t *p_data, uint32_t length)
{
    uint32_t read_length = length;
    uint32_t ret_code = app_fifo_read(&m_imu_data_fifo, p_data, &read_length);
    if (read_length == IMU_DATA_LENGTH_IN_BYTE)
        return true;
}
bool imu_fifo_flush(void)
{
    app_fifo_flush(&m_imu_data_fifo);
}
static void imu_data_fifo_init(void)
{
    ret_code_t err_code;
    static uint8_t imu_data_buff[IMU_DATA_BUFF_SIZE]; // Buffer for NFC TX FIFO instance

    err_code = app_fifo_init(&m_imu_data_fifo, imu_data_buff, IMU_DATA_BUFF_SIZE);
    APP_ERROR_CHECK(err_code);
}
bool bmi270_init(void)
{
    imu_data_fifo_init();
    if (!bmi270_setup_config())
    {
        NRF_LOG_ERROR("bmi270_setup_config fail");
        return false;
    }

    if (!bmi270_set_performance_mode())
    {
        NRF_LOG_ERROR("bmi270_setup_config fail");
        return false;
    }
    return true;
}
static uint8_t p_data[12];
bool bmi270_read(void)
{
    ret_code_t ret_code;
    uint32_t write_length = IMU_DATA_LENGTH_IN_BYTE;
    // check chip id
    p_data[0] = BMI270_REG_ACC_X_LSB;
    ret_code = iic_write(BMI270_ADDR, p_data, 1);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 set read pointer fail");
        return false;
    }

    ret_code = iic_read(BMI270_ADDR, p_data, 12);
    if (NRF_SUCCESS != ret_code)
    {
        NRF_LOG_INFO("bmi270 read data fail");
        return false;
    }
    if (trans_status)
    {
        app_fifo_write(&m_imu_data_fifo, p_data, &write_length);
    }
    //    NRF_LOG_HEXDUMP_INFO(p_data, 12);
    return 0;
}
