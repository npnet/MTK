/* Copyright Statement:
 *
 * (C) 2005-2021  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal_wdt.h"
#include "hal.h"
#include "bt_gap_le.h"
#include "bt_connection_manager_internal.h"
#include "bt_connection_manager_utils.h"
#include "bt_device_manager_config.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_state_notify.h"


//#define BT_CM_POWER_PRE_POWER_OFF_CB_NUM    5

static bt_cm_power_state_t bt_cm_power_state = BT_CM_POWER_STATE_OFF;
static bt_cm_power_state_t bt_cm_target_state = BT_CM_POWER_STATE_OFF;
static bool bt_cm_power_reset_flag = false;
static bool bt_cm_power_dut_mode = false;
/* Begin : Just for AT cmd test */
static bool bt_cm_sys_power_off_flag = false;
static bool bt_cm_sys_power_reset_flag = false;
/* End */
extern bt_sink_srv_cm_context_t g_sink_srv_cm_context;

/*
typedef struct {
    bt_connection_manager_pre_power_off_callback_t cb;
    void *data;
} bt_connection_manager_pre_power_off_cb_t;

static bt_connection_manager_pre_power_off_cb_t pre_power_off_cb[BT_CM_POWER_PRE_POWER_OFF_CB_NUM] = {{NULL, NULL}};
*/

static void bt_connection_manager_power_off_force_callback(void *params)
{
    if (BT_CM_POWER_STATE_OFF_PENDING == bt_cm_power_state) {
        bt_status_t ret = BT_STATUS_SUCCESS;
        bt_cmgr_report_id("[BT_CM][POWER][I] Force to do bt power off", 0);
        /*
        for (uint32_t i = 0; i < BT_CM_POWER_PRE_POWER_OFF_CB_NUM; i++) {
            if (pre_power_off_cb[i].cb != NULL) {
                pre_power_off_cb[i].cb(pre_power_off_cb[i].data);
            }
        }
        */
        if (false == (bool)params && bt_cm_power_dut_mode == false && BT_STATUS_SUCCESS != bt_connection_manager_disconnect_all(true)) {
            bt_cmgr_report_id("[BT_CM][POWER][I] ACL link not disconnected", 0);
            return;
        }
        if (false == bt_cm_power_reset_flag) {
            if (BT_STATUS_SUCCESS != (ret = bt_power_off())) {
                bt_cmgr_report_id("[BT_CM][POWER][E] To do bt power off Fail status %x", 1, ret);
            }
        } else {
            if (BT_STATUS_SUCCESS != (ret = bt_power_reset(BT_POWER_RESET_SW))) {
                bt_cmgr_report_id("[BT_CM][POWER][E] To do bt power reset Fail status %x", 1, ret);
            }
        }
    }
}

bt_status_t bt_connection_manager_power_set_target_state(bt_cm_power_state_t state)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][POWER][I] Bt set target state %d", 1, state);

    bt_cm_target_state = state;
    if (BT_CM_POWER_STATE_OFF == state && BT_CM_POWER_STATE_ON == bt_cm_power_state) {
        ret = bt_connection_manager_power_off(false);
    } else if (BT_CM_POWER_STATE_ON == state && BT_CM_POWER_STATE_OFF == bt_cm_power_state) {
        ret = bt_connection_manager_power_on();
    }
    return ret;
}

bt_status_t bt_connection_manager_power_on()
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_bd_addr_t *local_addr = NULL;

    //bt_connection_manager_mutex_lock();
    bt_cmgr_report_id("[BT_CM][POWER][I] Bt power on ...", 0);
    if (BT_CM_POWER_STATE_ON_PENDING == bt_cm_power_state) {
        bt_cmgr_report_id("[BT_CM][POWER][I] BT power on is pending", 0);
        //bt_connection_manager_mutex_unlock();
        return BT_STATUS_SUCCESS;
    } else if (BT_CM_POWER_STATE_OFF != bt_cm_power_state) {
        bt_cmgr_report_id("[BT_CM][POWER][W] BT is not in POWER OFF state, cur state : %d", 1, bt_cm_power_state);
        //bt_connection_manager_mutex_unlock();
        return BT_CONNECTION_MANAGER_STATUS_FAIL;
    }
    local_addr = bt_connection_manager_device_local_info_get_local_address();
    bt_cm_power_state = BT_CM_POWER_STATE_ON_PENDING;
    if (BT_STATUS_SUCCESS != (ret = bt_power_on((bt_bd_addr_ptr_t)local_addr, NULL))) {
        bt_cm_power_state = BT_CM_POWER_STATE_OFF;
        bt_cmgr_report_id("[BT_CM][POWER][W] Bt power on fail status 0x%08x", 1, ret);
    }
    //bt_connection_manager_mutex_unlock();
    return ret;
}

bt_status_t bt_connection_manager_power_off(bool force)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    uint32_t power_off_delay = 0;

    bt_connection_manager_mutex_lock();
    bt_cmgr_report_id("[BT_CM][POWER][I] Bt power off ...", 0);
    if (BT_CM_POWER_STATE_OFF == bt_cm_power_state || BT_CM_POWER_STATE_ON_PENDING == bt_cm_power_state) {
        bt_cmgr_report_id("[BT_CM][POWER][W] BT is not in correct state, cur state : %d", 1, bt_cm_power_state);
        bt_connection_manager_mutex_unlock();
        return BT_CONNECTION_MANAGER_STATUS_FAIL;
    }
    if (true == bt_connection_manager_timer_is_exist(BT_SINK_SRV_CM_TO_DO_BT_POWER_OFF_TIMER_ID)) {
        bt_connection_manager_timer_stop(BT_SINK_SRV_CM_TO_DO_BT_POWER_OFF_TIMER_ID);
    }
    bt_cm_power_state = BT_CM_POWER_STATE_OFF_PENDING;
    if (true == force || bt_cm_power_dut_mode == true || BT_STATUS_SUCCESS == bt_connection_manager_disconnect_all(false)) {
        power_off_delay = 10;
    } else {
        power_off_delay = 3000;
    }
    bt_connection_manager_timer_start(BT_SINK_SRV_CM_TO_DO_BT_POWER_OFF_TIMER_ID,
                                      power_off_delay, bt_connection_manager_power_off_force_callback, (void *)force);
    bt_connection_manager_mutex_unlock();
    return ret;
}

bt_status_t bt_connection_manager_power_reset(bool force)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_cmgr_report_id("[BT_CM][POWER][I] Bt power reset ...", 0);
    bt_connection_manager_mutex_lock();
    if (bt_cm_power_dut_mode == false && BT_CM_POWER_STATE_ON == bt_cm_target_state) {
        bt_cm_power_reset_flag = true;
        ret = bt_connection_manager_power_off(force);
    }
    bt_connection_manager_mutex_unlock();
    return ret;
}

bt_status_t bt_connection_manager_power_cancel_reset()
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][POWER][I] Cancel bt power reset, cur state 0x%02x, target state 0x%02x",
                      1, bt_cm_power_state, bt_cm_target_state);
    bt_connection_manager_mutex_lock();
    if (BT_CM_POWER_STATE_OFF == bt_cm_target_state ||
        (bt_cm_power_state != BT_CM_POWER_STATE_OFF_PENDING && true != bt_cm_power_reset_flag)) {
        ret = BT_STATUS_FAIL;
    } else {
        bt_cm_power_reset_flag = false;
        bt_cm_power_state = BT_CM_POWER_STATE_ON;
        if (true == bt_connection_manager_timer_is_exist(BT_SINK_SRV_CM_TO_DO_BT_POWER_OFF_TIMER_ID)) {
            bt_connection_manager_timer_stop(BT_SINK_SRV_CM_TO_DO_BT_POWER_OFF_TIMER_ID);
        }
    }
    bt_connection_manager_mutex_unlock();
    return ret;
}

bt_cm_power_state_t bt_connection_manager_power_get_state(void)
{
    return bt_cm_power_state;
}

bt_status_t bt_connection_manager_power_sys_off()
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][POWER][I] For test, system power off ...", 0);
    if (BT_CM_POWER_STATE_OFF == bt_cm_power_state) {
#ifdef HAL_RTC_FEATURE_RTC_MODE
        if (HAL_RTC_STATUS_ERROR == hal_rtc_enter_rtc_mode()) {
            bt_connection_manager_assert(0 && "Enter RTC mode fail !!!");
        }
#else /* #ifdef HAL_RTC_FEATURE_RTC_MODE */
#if 0 //def HAL_PMU_MODULE_ENABLED
        pmu_enter_power_off();
#endif /* #if 0 //def HAL_PMU_MODULE_ENABLED */
#endif /* #ifdef HAL_RTC_FEATURE_RTC_MODE */
    } else {
        ret = bt_connection_manager_power_off(false);
        bt_cm_sys_power_off_flag = (ret == BT_STATUS_SUCCESS ? true : false);
    }
    return ret;
}

bt_status_t bt_connection_manager_power_sys_reset()
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][POWER][I] For test, system power reset ... state %d", 1, bt_cm_power_state);
    if (BT_CM_POWER_STATE_OFF == bt_cm_power_state) {
#ifdef HAL_WDT_MODULE_ENABLED
        hal_wdt_config_t wdt_config;
        wdt_config.mode = HAL_WDT_MODE_RESET;
        wdt_config.seconds = 1;
        hal_wdt_init(&wdt_config);
        hal_wdt_software_reset();
#endif /* #ifdef HAL_WDT_MODULE_ENABLED */
    } else {
        ret = bt_connection_manager_power_off(false);
        bt_cm_sys_power_reset_flag = (ret == BT_STATUS_SUCCESS ? true : false);
    }
    return ret;
}

bt_status_t bt_connection_manager_power_event_handle(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][POWER][I] Bt power event handle msg 0x%08x status 0x%08x", 2, msg, status);
    switch (msg) {
        case BT_POWER_ON_CNF:
            bt_cm_power_dut_mode = false;
            bt_cm_power_state = BT_CM_POWER_STATE_ON;
            if (bt_cm_power_reset_flag == false && BT_CM_POWER_STATE_ON == bt_cm_target_state) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_POWER_ON);
            }
            bt_cm_power_reset_flag = false;
            if (BT_CM_POWER_STATE_OFF == bt_cm_target_state) {
                bt_connection_manager_power_off(false);
            }
            break;
        case BT_POWER_OFF_CNF:
            bt_cm_power_state = BT_CM_POWER_STATE_OFF;
            bt_cm_power_dut_mode = false;
            bt_connection_manager_timer_stop(BT_SINK_SRV_CM_TO_DO_BT_POWER_OFF_TIMER_ID);
            if (false == bt_cm_power_reset_flag && BT_CM_POWER_STATE_OFF == bt_cm_target_state) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_NONE);
            }
            if (true == bt_cm_sys_power_off_flag) {
                bt_connection_manager_power_sys_off();
            } else if (true == bt_cm_sys_power_reset_flag) {
                bt_connection_manager_power_sys_reset();
            } else if (BT_CM_POWER_STATE_ON == bt_cm_target_state && false == bt_cm_power_reset_flag) {
                bt_connection_manager_power_on();
            }
            break;
        case BT_DUT_MODE_ACTIVE_IND:
            bt_cm_power_dut_mode = true;
            break;
        default:
            break;
    }
    return result;
}

/*
bt_status_t bt_connection_manager_power_unregister_pre_power_off_cb(
    bt_connection_manager_pre_power_off_callback_t cb)
{
    bt_cmgr_report_id("[BT_CM][POWER][I] Unegister pre power off cb", 0);
    for (uint32_t i = 0; i < BT_CM_POWER_PRE_POWER_OFF_CB_NUM; i++) {
        if (pre_power_off_cb[i].cb == cb) {
            pre_power_off_cb[i].cb = NULL;
        }
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_connection_manager_power_register_pre_power_off_cb(
    bt_connection_manager_pre_power_off_callback_t cb, void* data)
{
    bt_connection_manager_power_unregister_pre_power_off_cb(cb);
    bt_cmgr_report_id("[BT_CM][POWER][I] Register pre power off cb", 0);
    for (uint32_t i = 0; i < BT_CM_POWER_PRE_POWER_OFF_CB_NUM; i++) {
        if (NULL == pre_power_off_cb[i].cb) {
            pre_power_off_cb[i].cb = cb;
            pre_power_off_cb[i].data = data;
            return BT_STATUS_SUCCESS;
        }
    }
    return BT_CONNECTION_MANAGER_STATUS_FAIL;
}
*/

