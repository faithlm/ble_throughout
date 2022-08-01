/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 15:36:50
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-07-30 16:16:16
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\inc\iic_peripheral.H
 * @Description: 
 * 
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved. 
 */
#ifndef _IIC_PERIPHERAL_H
#define _IIC_PERIPHERAL_H

#include "nordic_common.h"
#include "sdk_errors.h"
#include "stdint.h"
ret_code_t iic_write(uint8_t addr, uint8_t const * pdata, uint8_t size);

ret_code_t iic_read(uint8_t addr, uint8_t const * pdata, uint8_t size);

ret_code_t iic_master_init(void);

#endif

