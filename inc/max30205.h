/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:35:52
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-07-30 16:41:03
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\inc\max30205.h
 * @Description: 
 * 
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved. 
 */
#ifndef _MAX30205_H
#define _MAX30205_H

#include "user_log.h"

bool max_30205_init(void);
uint16_t max_30205_read(void);
#endif
