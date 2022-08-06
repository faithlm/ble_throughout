/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 14:35:52
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-06 01:31:24
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\inc\ads1299.h
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */
#ifndef _ADS1299_H
#define _ADS1299_H

#include "user_log.h"

#define RDATAC 0x10
#define SDATAC 0x11
#define RDATA 0x12

#define RREG 0x20
#define WREG 0x40

#define ID 0x00
#define CONFIG1 0x01
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF 0x04
#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C
#define RLD_SENSP 0x0D
#define RLD_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP 0x11

#define LOFF_STATP 0x12
#define LOFF_STATN 0x13

#define GPIO 0x14
#define PACE 0x15
#define RESP 0x16
#define CONFIG4 0x17
#define WCT1 0x18
#define WCT2 0x19

int ads_1299_init(void);
void ads_1299_start(void);
#endif
