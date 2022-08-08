/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:28:18
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-08 07:55:29
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\src\m_bmi270.c
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#include "iic_peripheral.h"
#include "nrf_delay.h"
#include "app_fifo.h"
#include "customer_ble.h"

#include "stdio.h"
#include "bmi270.h"
#include "common.h"
#include "m_bmi270.h"
#include "user_log.h"
#define BMI270_ADDR (0x68)
#define BMI270_WR_LEN (256)
#define BMI270_CONFIG_FILE_RETRIES (15)
#define BMI270_CONFIG_FILE_POLL_PERIOD_US (10000)
#define BMI270_INTER_WRITE_DELAY_US (1000)

#define GRAVITY_EARTH (9.80665f)

#define IMU_DATA_BUFF_SIZE 8192
static app_fifo_t m_imu_data_fifo;

bool get_imu_fifo_data(uint8_t *p_data, uint32_t length)
{
//    uint32_t read_length = length;
//    uint32_t ret_code = app_fifo_read(&m_imu_data_fifo, p_data, &read_length);
//    if (read_length == IMU_DATA_LENGTH_IN_BYTE)
//        return true;
		return false;
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

/*!
 * @brief This internal API is used to set configurations for accel.
 */
static int8_t set_accel_config(struct bmi2_dev *bmi2_dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer configuration. */
    struct bmi2_sens_config config;

    /* Configure the type of feature. */
    config.type = BMI2_ACCEL;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_get_sensor_config(&config, 1, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config.cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config.cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config.cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config.cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel configurations. */
        rslt = bmi270_set_sensor_config(&config, 1, bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        /* Map data ready interrupt to interrupt pin. */
        rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi2_dev);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}

/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
static float lsb_to_mps2(int16_t val, float g_range, uint8_t bit_width)
{
    float half_scale = ((float)(1 << bit_width) / 2.0f);

    return (GRAVITY_EARTH * val * g_range) / half_scale;
}
struct bmi2_dev bmi2_dev;

bool m_bmi270_init(void)
{
		int8_t rslt;
		uint8_t sensor_list = BMI2_ACCEL;
    imu_data_fifo_init();
			
	if (iic_master_init())
    {
        NRF_LOG_INFO("iic_master_init fail");
        return false;
    }
		
    rslt = bmi2_interface_init(&bmi2_dev, BMI2_I2C_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270. */
    rslt = bmi270_init(&bmi2_dev);
    bmi2_error_codes_print_result(rslt);
    if (rslt == BMI2_OK)
    {
        /* Accel configuration settings. */
        rslt = set_accel_config(&bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK)
        {
            /* NOTE:
             * Accel enable must be done after setting configurations
             */
            rslt = bmi270_sensor_enable(&sensor_list, 1, &bmi2_dev);
            bmi2_error_codes_print_result(rslt);

            NRF_LOG_INFO("Accel and m/s2 data \n");
            NRF_LOG_INFO("Accel data collected at 2G Range with 16-bit resolution\n");

            /* Loop to print the accel data when interrupt occurs. */
        }
    }
    // if (!bmi270_setup_config())
    // {
    //     NRF_LOG_ERROR("bmi270_setup_config fail");
    //     return false;
    // }

    // if (!bmi270_set_performance_mode())
    // {
    //     NRF_LOG_ERROR("bmi270_setup_config fail");
    //     return false;
    // }
    return true;
}
static uint8_t p_data[12];
bool bmi270_read(void)
{
    // ret_code_t ret_code;
    // uint32_t write_length = IMU_DATA_LENGTH_IN_BYTE;
    // // check chip id
    // p_data[0] = BMI270_REG_ACC_X_LSB;
    // ret_code = iic_write(BMI270_ADDR, p_data, 1);
    // if (NRF_SUCCESS != ret_code)
    // {
    //     NRF_LOG_INFO("bmi270 set read pointer fail");
    //     return false;
    // }

    // ret_code = iic_read(BMI270_ADDR, p_data, 12);
    // if (NRF_SUCCESS != ret_code)
    // {
    //     NRF_LOG_INFO("bmi270 read data fail");
    //     return false;
    // }
    // if (trans_status)
    // {
    //     app_fifo_write(&m_imu_data_fifo, p_data, &write_length);
    // }
    // //    NRF_LOG_HEXDUMP_INFO(p_data, 12);
    // return 0;
    uint8_t tbuf[1];
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Variable to define limit to print accel data. */
    uint8_t limit = 20;

    /* Assign accel sensor to variable. */
    uint8_t sensor_list = BMI2_ACCEL;

    /* Sensor initialization configuration. */
    

    /* Create an instance of sensor data structure. */
    struct bmi2_sens_data sensor_data = {{0}};

    /* Initialize the interrupt status of accel. */
    uint16_t int_status = 0;

    uint8_t indx = 0;
    float x = 0, y = 0, z = 0;

    rslt = bmi2_get_int_status(&int_status, &bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* To check the accel data ready interrupt status and print the status for 10 samples. */
    if (int_status & BMI2_ACC_DRDY_INT_MASK)
    {
        /* Get accelerometer data for x, y and z axis. */
        rslt = bmi2_get_sensor_data(&sensor_data, &bmi2_dev);
        bmi2_error_codes_print_result(rslt);
        // NRF_LOG_INFO("\nAcc_X = %d\t", sensor_data.acc.x);
        // NRF_LOG_INFO("Acc_Y = %d\t", sensor_data.acc.y);
        // NRF_LOG_INFO("Acc_Z = %d\r\n", sensor_data.acc.z);

        /* Converting lsb to meter per second squared for 16 bit accelerometer at 2G range. */
        x = lsb_to_mps2(sensor_data.acc.x, 2, bmi2_dev.resolution);
        y = lsb_to_mps2(sensor_data.acc.y, 2, bmi2_dev.resolution);
        z = lsb_to_mps2(sensor_data.acc.z, 2, bmi2_dev.resolution);

        /* Print the data in m/s2. */
        // NRF_LOG_INFO("\nAcc_ms2_X = %4.2f, Acc_ms2_Y = %4.2f, Acc_ms2_Z = %4.2f\r\n", x, y, z);
        NRF_LOG_INFO("%4.2f,%4.2f,%4.2f\r\n", x, y, z);
        // NRF_LOG_INFO("\r\n");
        indx++;
    }
}
