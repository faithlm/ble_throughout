/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:28:18
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-03 01:03:34
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\src\bmi270.c
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#include "bmi270.h"
#include "iic_peripheral.h"
#include "nrf_delay.h"
#include "bmi270_config_file.h"

#define BMI270_ADDR (0x68)
#define BMI270_WR_LEN (256)
#define BMI270_CONFIG_FILE_RETRIES (15)
#define BMI270_CONFIG_FILE_POLL_PERIOD_US (10000)
#define BMI270_INTER_WRITE_DELAY_US (1000)

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

bool bmi270_init(void)
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
    NRF_LOG_INFO("bmi270 chip id read success");

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
            return ret_code;
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

uint16_t bmi270_read(void)
{
    // ret_code_t ret_code;

    // p_data_read[0] = TEMP_REGISTER;
    // ret_code = iic_write(BMI270_ADDR, p_data_read, 1);
    // if (NRF_SUCCESS != ret_code)
    // {
    //     NRF_LOG_INFO("iic_write err = %d", ret_code);
    //     return false;
    // }
    return 0;
}
