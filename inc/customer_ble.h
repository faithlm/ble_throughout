/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:35:52
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-08 03:55:20
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\inc\customer_ble.h
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#ifndef _BLE_H
#define _BLE_H

#define BLE_COMMAND_TRANS_CONTROL (0x01)
#define BLE_COMMAND_ADC_SAMP_FREQ (0x02)

#define BLE_TRANS_START (0x01)
#define BLE_TRANS_STOP (0x02)

void ble_init(void);
extern bool trans_status;
#endif
