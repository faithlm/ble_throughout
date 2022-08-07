/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:35:52
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-08 07:05:30
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\inc\max30205.h
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#ifndef _MAX30205_H
#define _MAX30205_H

#include "user_log.h"

#define TEMP_DATA_LENGTH_IN_BYTE (2)

bool max_30205_init(void);
uint16_t max_30205_read(void);
bool get_temp_fifo_data(uint8_t *p_data, uint32_t length);
bool temp_fifo_flush(void);
#endif
