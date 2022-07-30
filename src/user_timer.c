/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 15:17:04
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-07-30 15:20:42
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\src\user_timer.c
 * @Description: 
 * 
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved. 
 */

#include "app_timer.h"

void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}
