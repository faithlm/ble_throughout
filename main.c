/*
 * @Author: Liangmeng
 * @Date: 2022-07-30 11:54:20
 * @LastEditors: Liangmeng
 * @LastEditTime: 2022-08-08 07:24:11
 * @FilePath: \nRF5_SDK_17.1.0_ddde560\examples\ble_peripheral\ble_throughout\main.c
 * @Description:
 *
 * Copyright (c) 2022 by DESKTOP-JDCNG1F\Administrator liangmeng0011@163.com, All Rights Reserved.
 */

#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "user_log.h"
#include "customer_ble.h"
#include "max30205.h"
#include "bmi270.h"
#include "app_scheduler.h"
#include "ads1299.h"
#include "customer_ble.h"
#define APP_SCHED_MAX_EVENT_SIZE (20) /**< Maximum size of scheduler events. */
#define APP_SCHED_QUEUE_SIZE (50)     /**< Maximum number of events in the scheduler queue. */

#define TEMP_MEAS_INTERVAL APP_TIMER_TICKS(100)
APP_TIMER_DEF(m_temp_timer_id);
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

/**
 * @brief Function for updating NDEF message in the flash file.
 */
static void scheduler_read_temp(void *p_event_data, uint16_t event_size)
{
    UNUSED_PARAMETER(p_event_data);
    UNUSED_PARAMETER(event_size);

    max_30205_read();
}

/**
 * @brief Function for updating NDEF message in the flash file.
 */
static void scheduler_read_imu(void *p_event_data, uint16_t event_size)
{
    UNUSED_PARAMETER(p_event_data);
    UNUSED_PARAMETER(event_size);

    bmi270_read();
}

static void temp_meas_timeout_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);
    ret_code_t err_code;
    static int32_t m_100ms_cnt = 0;
    m_100ms_cnt++;
    NRF_LOG_INFO("Cnt = %d", get_cnt());
    err_code = app_sched_event_put(NULL, 0, scheduler_read_imu);
    APP_ERROR_CHECK(err_code);

    if (0 == m_100ms_cnt % 10)
    {
        m_100ms_cnt = 0;
        err_code = app_sched_event_put(NULL, 0, scheduler_read_temp);
        APP_ERROR_CHECK(err_code);
    }
}

void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_temp_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                temp_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Start application timers.
    err_code = app_timer_start(m_temp_timer_id, TEMP_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Application main function.
 */
int main(void)
{

    log_init();
    timers_init();
    /* Initialize App Scheduler. */
    APP_SCHED_INIT(APP_SCHED_MAX_EVENT_SIZE, APP_SCHED_QUEUE_SIZE);
    power_management_init();
    ble_init();

    if (!bmi270_init())
    {
        NRF_LOG_ERROR("bmi270 init fail");
    }

    if (!max_30205_init())
    {
        NRF_LOG_ERROR("max_30205_init fail");
    }
    ads_1299_init();
    ads_1299_start();
    // Enter main loop.
    for (;;)
    {
        app_sched_execute();
        idle_state_handle();
    }
}
