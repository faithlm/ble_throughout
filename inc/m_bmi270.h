/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:35:52
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-08 07:58:50
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\inc\m_bmi270.h
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#ifndef _BMI270_H
#define _BMI270_H

#include "user_log.h"

bool get_imu_fifo_data(uint8_t *p_data, uint32_t length);
bool imu_fifo_flush(void);
bool m_bmi270_init(void);
bool bmi270_read(void);
#endif
