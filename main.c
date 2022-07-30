/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 11:54:20
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-07-30 15:22:24
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\main.c
 * @Description: 
 * 
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved. 
 */

#include "nrf_pwr_mgmt.h"
#include "user_timer.h"
#include "user_log.h"
#include "customer_ble.h"

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

/**@brief Application main function.
 */
int main(void)
{
  
    log_init();
    timers_init();
	power_management_init();
    ble_init();
    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}
