/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */



#include "FreeRTOS.h"
#include "bt_type.h"
#include "timers.h"
#include "bt_connection_manager.h"
#include "bt_timer_external.h"

#include "bt_le_audio_msglog.h"
#include "bt_le_audio_sink.h"
#include "bt_le_audio_util.h"
#include "bt_sink_srv_le_cap_stream.h"
#include "bt_sink_srv_le_volume.h"
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "ble_bass.h"
#ifdef AIR_LE_AUDIO_CIS_ENABLE
#include "ble_ascs.h"
#include "ble_bass.h"
#include "ble_pacs.h"
#endif /* AIR_LE_AUDIO_CIS_ENABLE */
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd_le_audio.h"
#endif



/**************************************************************************************************
* Enum
**************************************************************************************************/
#define CAP_INVALID_UINT8   0xFF

#ifdef AIR_LE_AUDIO_CIS_ENABLE
#define CAP_MAX_ASE_NUM     MAX_ASE_NUM
#define CAP_MAX_CIS_NUM	    4

#define DEFAULT_ASE_ID_1        0x01//Media(Sink)
#define DEFAULT_ASE_ID_2        0x02//Call_downlink(Sink)
#define DEFAULT_ASE_ID_3        0x03//Call_uplink(Source)

#ifdef MTK_LE_AUDIO_CT
#define MAX_STREAMING_ASE_NUM        3 // 2 Sink + 1 Source
#else //MTK_COMMON
#define MAX_STREAMING_ASE_NUM        2 // 2 Sink
#endif

#define BT_PBP_UUID16_PUBLIC_BROADCAST_ANNOUNCEMENTS_SERVICE    0x1856      /**< Public Broadcast Announcement Service UUID. */
#define BT_GAP_LE_AD_TYPE_BROADCAST_NAME                        0x30        /**< Broadcast Name, Public broadcast Profile */

enum {
    SUB_STATE_ENABLING_RESPONDING,
    SUB_STATE_ENABLING_NOTIFYING,
    SUB_STATE_DISABLING_RESPONDING,
    SUB_STATE_RELEASING_RESPONDING,
    SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING,
    SUB_STATE_CODEC_CONFIG_RESPONDING,
    SUB_STATE_QOS_CONFIG_RESPONDING,
    SUB_STATE_SETTING_DATA_PATH
};
#endif

/**************************************************************************************************
* Structure
**************************************************************************************************/
typedef struct {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
    uint8_t max_link_num;
    bt_handle_t service_connect_handle;
    bt_handle_t le_handle_with_cis_established;
    bt_handle_t handle_to_disconnect;
#endif
    bt_sink_srv_cap_stream_service_big_t big_info;
} PACKED bt_sink_srv_cap_stream_service_info_t;

#ifdef AIR_LE_AUDIO_CIS_ENABLE
typedef struct {
    uint8_t head;
    uint8_t tail;
    uint8_t queue[CAP_MAX_ASE_NUM];
} PACKED bt_sink_srv_cap_stream_ase_queue_info_t;

typedef struct {
    uint8_t ase_id;
    uint8_t ase_state;
    uint8_t sub_state;
    uint8_t next_sub_state;
    uint8_t codec[AUDIO_CODEC_ID_SIZE];
    uint8_t direction;
    uint8_t target_latency;
    uint8_t codec_specific_configuration_length;
    uint8_t *codec_specific_configuration;
    //bap_ase_qos_config_ind_t qos_config;
    uint8_t  cig_id;
    uint8_t  cis_id;
    uint8_t  sdu_interval[SDU_INTERVAL_SIZE];
    uint8_t  framing;
    uint8_t  phy;
    uint16_t maximum_sdu_size;
    uint8_t  retransmission_number;
    uint16_t transport_latency;
    uint8_t presentation_delay[3];
    bt_handle_t cis_handle;
    uint16_t iso_interval;
    uint8_t metadata_length;
    uint8_t *metadata;
} PACKED bt_sink_srv_cap_stream_ase_info_t;

typedef struct {
    bt_sink_srv_cap_stream_ase_info_t ase_info[CAP_MAX_ASE_NUM];
} PACKED bt_sink_srv_cap_stream_link_info_t;

typedef struct
{
    bt_handle_t le_handle;
    uint8_t ase_id;
    uint8_t reserve;
}PACKED bt_sink_srv_cap_stream_send_ase_streaming_state_data_t;

#endif

typedef struct {
    uint8_t length;
    uint8_t ad_type;
    uint16_t uuid;
    uint8_t pb_announcement_features;
    uint8_t metadata_length;
    uint8_t metadata[1];
} PACKED bt_sink_srv_cap_stream_pb_announcement_t;

/**************************************************************************************************
* Variables
**************************************************************************************************/
#ifdef AIR_LE_AUDIO_CIS_ENABLE
static bt_sink_srv_cap_stream_link_info_t *gp_cap_stream_link_info;
static TimerHandle_t g_cap_stream_am_timer = NULL;
//static TimerHandle_t g_cap_stream_cis_disconnection_timer = NULL;
#endif

static bt_sink_srv_cap_stream_service_info_t g_cap_stream_service_info;
static bool g_broadcast_mute = false;
static bt_sink_srv_am_volume_level_out_t g_broadcast_volume = AUD_VOL_OUT_LEVEL7;
static bt_sink_srv_cap_stream_bmr_scan_info_t g_default_bmr_scan_info;
static bt_sink_srv_cap_stream_send_ase_streaming_state_data_t g_ase_streaming_timer_data[CAP_MAX_CIS_NUM];
static bool g_broadcast_keep_pa = true;

extern bool g_cap_am_switch_suspending;
//(MTK_COMMON: Why use internal fun in bt_le_audio_protected?)extern bt_status_t le_audio_sink_inform_app(bt_le_audio_sink_event_t event, void *msg);
/**************************************************************************************************
* Prototype
**************************************************************************************************/
static void bt_sink_srv_cap_stream_callback(uint8_t event_id, void *p_msg);
static void bt_sink_srv_cap_stream_parameter_init(uint8_t max_link_num);
#ifdef AIR_LE_AUDIO_CIS_ENABLE
static bool bt_sink_srv_cap_stream_set_ase_link(bt_handle_t connect_handle, uint8_t ase_id);
static bool bt_sink_srv_cap_stream_is_ase_link_set(bt_handle_t connect_handle, uint8_t ase_id);
static bt_sink_srv_cap_stream_ase_info_t *bt_sink_srv_cap_stream_get_ase_link(bt_handle_t connect_handle, uint8_t ase_id);
static bt_sink_srv_cap_stream_ase_info_t *bt_sink_srv_cap_stream_get_ase_link_by_cis_handle(bt_handle_t cis_handle);
//static bool bt_sink_srv_cap_stream_clear_ase_link(bt_handle_t connect_handle, uint8_t ase_id);
static bool bt_sink_srv_cap_stream_state_check(uint8_t current_state, uint8_t next_state);
static bool bt_sink_srv_cap_stream_sub_state_check(uint8_t current_sub_state, uint8_t current_state);
//static bt_sink_srv_cap_am_mode bt_sink_srv_cap_stream_get_mode_by_connection_handle(bt_handle_t connect_handle);
static bool bt_sink_srv_cap_stream_set_ase_state(bt_handle_t connect_handle, uint8_t ase_id, uint8_t state);
static bool bt_sink_srv_cap_stream_set_ase_sub_state(bt_handle_t connect_handle, uint8_t ase_id, uint8_t sub_state);
static bool bt_sink_srv_cap_stream_set_ase_next_sub_state(bt_handle_t connect_handle, uint8_t ase_id, uint8_t next_sub_state);
static uint8_t bt_sink_srv_cap_stream_get_ase_state(bt_handle_t connect_handle, uint8_t ase_id);
static uint8_t bt_sink_srv_cap_stream_get_ase_sub_state(bt_handle_t connect_handle, uint8_t ase_id);
static bool bt_sink_srv_cap_stream_check_and_update_ase_next_sub_state(bt_handle_t connect_handle, uint8_t ase_id);
static uint8_t bt_sink_srv_cap_stream_get_ase_direction(bt_handle_t connect_handle, uint8_t ase_id);
//static uint8_t bt_sink_srv_cap_stream_get_ase_id_by_direction(bt_handle_t connect_handle, uint8_t direction);
static ble_bap_ase_id_list_t bt_sink_srv_cap_stream_find_ase_id_list_with_cis_established(bt_handle_t connect_handle);
static uint8_t bt_sink_srv_cap_stream_get_cis_list(bt_handle_t connect_handle, bt_handle_t *cis_list);
//static uint8_t bt_sink_srv_cap_stream_get_uplink_cis_list(bt_handle_t connect_handle, bt_handle_t *cis_list);
//static bool bt_sink_srv_cap_stream_clear_service_big(void);
static bool bt_sink_srv_cap_stream_clear_cis_handle(bt_handle_t connect_handle, uint8_t ase_id);
static bool bt_sink_srv_cap_stream_is_ble_link_serviced(bt_handle_t connect_handle);
static bool bt_sink_srv_cap_stream_set_codec(ble_bap_ase_codec_config_ind_t *p_msg);
static bool bt_sink_srv_cap_stream_set_qos(ble_bap_ase_qos_config_ind_t *p_msg);
static bool bt_sink_srv_cap_stream_set_metadata(ble_bap_ase_enable_ind_t *p_msg);
static bool bt_sink_srv_cap_stream_set_cis_handle(bt_handle_t connect_handle, uint8_t ase_id, bt_handle_t cis_handle);
static bt_handle_t bt_sink_srv_cap_stream_get_cis_handle(bt_handle_t connect_handle, uint8_t ase_id);
static void bt_sink_srv_cap_stream_state_notify_handler(ble_bap_ase_state_notify_t *p_msg);
static void bt_sink_srv_cap_stream_codec_config_ind_handler(ble_bap_ase_codec_config_ind_t *p_msg);
static void bt_sink_srv_cap_stream_qos_config_ind_handler(ble_bap_ase_qos_config_ind_t *p_msg);
static void bt_sink_srv_cap_stream_ase_enabling_ind_handler(ble_bap_ase_enable_ind_t *p_msg);
static void bt_sink_srv_cap_stream_cis_requeset_ind_handler(ble_bap_cis_established_ind_t *p_msg);
static void bt_sink_srv_cap_stream_cis_established_notify_handler(ble_bap_cis_established_notify_t *p_msg);
static void bt_sink_srv_cap_stream_ase_disabling_ind_handler(ble_bap_ase_disable_ind_t *p_msg);
static void bt_sink_srv_cap_stream_cis_disconnected_notify_handler(ble_bap_cis_disconnected_notify_t *p_msg);
static void bt_sink_srv_cap_stream_ase_releasing_ind_handler(ble_bap_ase_release_ind_t *p_msg);
static void bt_sink_srv_cap_stream_ase_receiver_start_ready_ind_handler(ble_bap_ase_receiver_start_ready_ind_t *p_msg);
static void bt_sink_srv_cap_stream_ase_receiver_stop_ready_ind_handler(ble_bap_ase_receiver_stop_ready_ind_t *p_msg);
static void bt_sink_srv_cap_stream_ase_update_metadata_ind_handler(ble_bap_ase_update_metadata_ind_t *p_msg);
static bool bt_sink_srv_cap_stream_is_enabling_state_ok(uint8_t state);
static bool bt_sink_srv_cap_stream_is_disabling_state_ok(uint8_t state);
static bool bt_sink_srv_cap_stream_is_releasing_state_ok(uint8_t state);
static void bt_sink_srv_cap_stream_receiver_ready_handler(bt_handle_t connect_handle, uint8_t ase_id, bool is_start);
static uint8_t bt_sink_srv_cap_stream_pop_proccessing_ase_id(bt_handle_t connect_handle, bt_le_audio_direction_t direction, bool is_enabling);
#ifndef MTK_LE_CAP_STREAM_COMMON
static uint8_t bt_sink_srv_cap_stream_pop_second_proccessing_ase_id(bt_handle_t connect_handle, bt_le_audio_direction_t direction, bool is_enabling);
#endif
static bool bt_sink_srv_cap_stream_cis_disconnection_timer_start(bt_handle_t cis_handle);
static bool bt_sink_srv_cap_stream_send_ase_streaming_state_timer_start(bt_handle_t le_handle, uint8_t ase_id);
static bool bt_sink_srv_cap_stream_am_timer_stop(void);
static void bt_sink_srv_cap_stream_am_timer_callback(TimerHandle_t timer_id);
static bool bt_sink_srv_cap_stream_cis_disconnection_timer_stop(void);
static void bt_sink_srv_cap_stream_cis_disconnection_timer_callback(uint32_t timer_id, uint32_t data);
static bool bt_sink_srv_cap_stream_send_ase_streaming_state_timer_stop(bt_handle_t stop_handle);
static void bt_sink_srv_cap_stream_send_ase_streaming_state_timer_callback(uint32_t timer_id, uint32_t data);
//extern uint8_t bt_sink_srv_cap_get_link_index(bt_handle_t connect_handle);
#endif
static void bt_sink_srv_cap_stream_set_service_big(bt_handle_t sync_handle, uint8_t big_handle, uint8_t num_bis, uint8_t *bis_indices);
static void bt_sink_srv_cap_stream_big_sync_established_notify_handler(ble_bap_big_sync_established_notify_t *p_msg);

extern bool bt_sink_srv_cap_inform_app(bt_sink_srv_cap_event_id_t event_id, void *p_msg);

/**************************************************************************************************
* Static Functions
**************************************************************************************************/
static void bt_sink_srv_cap_stream_parse_pb_announcement_data(uint16_t data_length, uint8_t *p_adv)
{
    uint8_t acc_len, *p_broadcast_name;

    typedef struct {
        uint8_t len;
        uint8_t ad_type;
        uint8_t ad_data[1];
    } adv_t;

    acc_len = 0;

    while (acc_len < data_length) {
        adv_t *p_adv_data = (adv_t *)p_adv;
        bt_sink_srv_cap_stream_pb_announcement_t *p_pb_announcement = (bt_sink_srv_cap_stream_pb_announcement_t *)p_adv_data;

        if (BT_GAP_LE_AD_TYPE_SERVICE_DATA == p_adv_data->ad_type &&
            BT_PBP_UUID16_PUBLIC_BROADCAST_ANNOUNCEMENTS_SERVICE == p_pb_announcement->uuid) {
            le_audio_log("[CAP][stream] Public Broadcast Announcement: features:0x%x", 1, p_pb_announcement->pb_announcement_features);

        } else if (BT_GAP_LE_AD_TYPE_BROADCAST_NAME == p_adv_data->ad_type) {
            if (NULL != (p_broadcast_name = le_audio_malloc(p_adv_data->len))) {
                memset(p_broadcast_name, 0, p_adv_data->len);
                memcpy(p_broadcast_name, p_adv_data->ad_data, (p_adv_data->len - 1));
                LOG_I(LE_AUDIO, "[CAP][stream] PB Announcement Name: %s", p_broadcast_name);
                le_audio_free(p_broadcast_name);
            }
        }

        p_adv += (p_adv_data->len + 1);
        acc_len += (p_adv_data->len + 1);
    }
}

static void bt_sink_srv_cap_stream_callback(uint8_t event_id, void *p_msg)
{
    //le_audio_log("[CAP][stream] callback, event:%d", 1, event_id); //MTK_COMMON: David - reduce log
    switch (event_id) {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
        /*Unicast*/
        case BLE_BAP_ASE_STATE_NOTIFY:
            bt_sink_srv_cap_stream_state_notify_handler((ble_bap_ase_state_notify_t *)p_msg);
            break;

        case BLE_BAP_ASE_CODEC_CONFIG_IND:
            bt_sink_srv_cap_stream_codec_config_ind_handler((ble_bap_ase_codec_config_ind_t *)p_msg);
            break;

        case BLE_BAP_ASE_QOS_CONFIG_IND:
            bt_sink_srv_cap_stream_qos_config_ind_handler((ble_bap_ase_qos_config_ind_t *)p_msg);
            break;

        case BLE_BAP_ASE_ENABLE_IND:
            bt_sink_srv_cap_stream_ase_enabling_ind_handler((ble_bap_ase_enable_ind_t *)p_msg);
            break;

        case BLE_BAP_CIS_REQUEST_IND:
            bt_sink_srv_cap_stream_cis_requeset_ind_handler((ble_bap_cis_established_ind_t *)p_msg);
            break;

        case BLE_BAP_CIS_ESTABLISHED_NOTIFY:
            bt_sink_srv_cap_stream_cis_established_notify_handler((ble_bap_cis_established_notify_t *)p_msg);
            break;

        case BLE_BAP_ASE_DISABLE_IND:
            bt_sink_srv_cap_stream_ase_disabling_ind_handler((ble_bap_ase_disable_ind_t *)p_msg);
            break;

        case BLE_BAP_CIS_DISCONNECTED_NOTIFY:
            if (g_cap_stream_service_info.handle_to_disconnect != BT_HANDLE_INVALID) {
                ble_bap_disconnect_cis(g_cap_stream_service_info.handle_to_disconnect);
                g_cap_stream_service_info.handle_to_disconnect = BT_HANDLE_INVALID;
            }

            bt_sink_srv_cap_stream_cis_disconnected_notify_handler((ble_bap_cis_disconnected_notify_t *)p_msg);
            break;

        case BLE_BAP_ASE_RELEASE_IND:
            bt_sink_srv_cap_stream_ase_releasing_ind_handler((ble_bap_ase_release_ind_t *)p_msg);
            break;

        case BLE_BAP_ASE_RECEIVER_START_READY_IND:
            bt_sink_srv_cap_stream_ase_receiver_start_ready_ind_handler((ble_bap_ase_receiver_start_ready_ind_t *)p_msg);
            break;

        case BLE_BAP_ASE_RECEIVER_STOP_READY_IND:
            bt_sink_srv_cap_stream_ase_receiver_stop_ready_ind_handler((ble_bap_ase_receiver_stop_ready_ind_t *)p_msg);
            break;

        case BLE_BAP_ASE_UPDATE_METADATA_IND:
            bt_sink_srv_cap_stream_ase_update_metadata_ind_handler((ble_bap_ase_update_metadata_ind_t *)p_msg);
            break;
#endif
        /*Broadcast*/
        case BLE_BAP_BASE_BROADCAST_AUDIO_ANNOUNCEMENTS_IND: {
            bt_sink_srv_cap_event_base_broadcast_audio_announcements_t *p_broadcast = (bt_sink_srv_cap_event_base_broadcast_audio_announcements_t *)p_msg;

            le_audio_log("[CAP][stream] BROADCAST_AUDIO_ANNOUNCEMENTS type:%d, sid:%X, addr:%02X:%02X:%02X:%02X:%02X:%02X", 8,
                         p_broadcast->addr.type, p_broadcast->advertising_sid,
                         p_broadcast->addr.addr[5], p_broadcast->addr.addr[4],
                         p_broadcast->addr.addr[3], p_broadcast->addr.addr[2],
                         p_broadcast->addr.addr[1], p_broadcast->addr.addr[0]);

            bt_sink_srv_cap_stream_parse_pb_announcement_data((uint16_t)p_broadcast->data_length, p_broadcast->data);

            if (g_default_bmr_scan_info.specified_bms) {
                if (0 == memcmp(&g_default_bmr_scan_info.bms_address[0],
                                &p_broadcast->addr.addr[0], sizeof(bt_bd_addr_t))) {
                    le_audio_log("[CAP][stream] Match specified BMS", 0);
                    bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BROADCAST_AUDIO_ANNOUNCEMENTS, p_msg);
                }
            } else {
                bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BROADCAST_AUDIO_ANNOUNCEMENTS, p_msg);
            }
#if defined (AIR_LE_AUDIO_BIS_ENABLE) && defined (MTK_RACE_CMD_ENABLE) && !defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
            race_le_audio_notify_ea(0, p_msg);
#endif
            break;
        }

        case BLE_BAP_BASE_PERIODIC_ADV_SYNC_ESTABLISHED_NOTIFY:
            memcpy(g_default_bmr_scan_info.bms_address,
                   ((ble_bap_periodic_adv_sync_established_notify_t *)p_msg)->advertiser_addr.addr, sizeof(bt_bd_addr_t));
            g_default_bmr_scan_info.sync_handle = ((ble_bap_periodic_adv_sync_established_notify_t *)p_msg)->sync_handle;
            le_audio_log("[CAP][stream] BLE_BAP_BASE_PERIODIC_ADV_SYNC_ESTABLISHED_NOTIFY, status:0x%X", 1, ((ble_bap_periodic_adv_sync_established_notify_t *)p_msg)->status);
            bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_PERIODIC_ADV_SYNC_ESTABLISHED, p_msg);
            break;

        case BLE_BAP_BASE_BASIC_AUDIO_ANNOUNCEMENTS_IND:
            bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BASIC_AUDIO_ANNOUNCEMENTS, p_msg);
#if defined (AIR_LE_AUDIO_BIS_ENABLE) && defined (MTK_RACE_CMD_ENABLE) && !defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
            race_le_audio_notify_pa(0, p_msg);
#endif
            break;

        case BLE_BAP_BASE_BIGINFO_ADV_REPORT_IND:
            bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BIGINFO_ADV_REPORT, p_msg);
            break;

        case BLE_BAP_BASE_BIG_SYNC_IND:
            bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BIG_SYNC_IND, p_msg);
            break;

        case BLE_BAP_BASE_BIG_SYNC_ESTABLISHED_NOTIFY:
            bt_sink_srv_cap_stream_big_sync_established_notify_handler((ble_bap_big_sync_established_notify_t *)p_msg);
#if defined (AIR_LE_AUDIO_BIS_ENABLE) && defined (MTK_RACE_CMD_ENABLE) && !defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
            race_le_audio_notify_big_sync_ind(0);
#endif
            break;

        case BLE_BAP_BASE_PERIODIC_ADV_TERNIMATE_IND:
            g_default_bmr_scan_info.sync_handle = BT_HANDLE_INVALID;
            bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_PERIODIC_ADV_TERMINATE, p_msg);
            break;

        case BLE_BAP_BASE_PERIODIC_ADV_TERMINATE_CNF:
            break;
        case BLE_BAP_BASE_BIG_TERNIMATE_IND:
            le_audio_log("[CAP][stream] BLE_BAP_BASE_BIG_TERNIMATE_IND, BIG handle:0x%x", 1, ((ble_bap_big_terminate_ind_t *)p_msg)->big_handle);
            bt_sink_srv_cap_am_audio_stop(CAP_AM_BROADCAST_MUSIC_MODE);
            bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BIG_TERMINATE_IND, p_msg);
            break;

        case BLE_BAP_BASE_BIG_TERMINATE_CNF:
            break;

        case BLE_BAP_BASE_SCAN_TIMEOUT_IND: {
            bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_SCAN_TIMEOUT, p_msg);
#if defined (AIR_LE_AUDIO_BIS_ENABLE) && defined (MTK_RACE_CMD_ENABLE) && !defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
            bt_sink_srv_cap_event_base_broadcast_audio_announcements_t empty_baa = {{0}};
            race_le_audio_notify_ea(1, &empty_baa);
#endif
            break;
        }
        case BLE_BAP_BASE_BASS_ADD_SOURCE_IND: {
            ble_bap_bass_add_source_ind_t *ind = (ble_bap_bass_add_source_ind_t *)p_msg;

            le_audio_log("[CAP][stream] Add source, type:%d, sid:%X, pa:%x, source_id:%d", 4,
                         ind->param->advertising_addr_type, ind->param->advertising_sid, ind->param->pa_sync, ind->source_id);

            if (ind->param->pa_sync) {
                if (bt_sink_srv_get_state() >= BT_SINK_SRV_STATE_INCOMING || bt_sink_srv_cap_am_get_current_mode() <= CAP_AM_UNICAST_CALL_MODE_END) {
                    /*Call is doing, shall not be interrupted*/
                    le_audio_log("[CAP][stream] Add source FAIL, call is active", 0);
                    ble_bap_bass_add_source_response(ind->connect_handle, ind->source_id, false);
                    break;
                }

                /*Inform APP to disable sync feature because BSA is used*/
                bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BASS_ADD_SOURCE, p_msg);

                if (ind->param->pa_sync == BT_BASS_PA_SYNC_SYNCHRONIZE_TO_PA_PAST_AVAILABLE) {
                    ble_bap_bass_add_source_response(ind->connect_handle, ind->source_id, true);
                } else if (ind->param->pa_sync == BT_BASS_PA_SYNC_SYNCHRONIZE_TO_PA_PAST_NOT_AVAILABLE) {
                    bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
                    scan_param.audio_channel_allocation = ble_pacs_get_audio_location(AUDIO_DIRECTION_SINK);
                    scan_param.bms_address = &ind->param->advertiser_addr;
                    scan_param.duration = DEFAULT_SCAN_TIMEOUT;
                    bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
                }
            }
            break;
        }
        case BLE_BAP_BASE_BASS_MODIFY_SOURCE_IND: {
            ble_bap_bass_modify_source_ind_t *ind = (ble_bap_bass_modify_source_ind_t *)p_msg;
            le_audio_log("[CAP][stream] Modify source, source_id:%d, pa_sync:%d, sub_group:%d, bis_sync:%04X", 4,
                         ind->source_id, ind->param->pa_sync, ind->param->subgroup, ind->param->bis_sync);
            if (ind->param->pa_sync) {
                if (bt_sink_srv_get_state() >= BT_SINK_SRV_STATE_INCOMING || bt_sink_srv_cap_am_get_current_mode() <= CAP_AM_UNICAST_CALL_MODE_END) {
                    /*Call is doing, shall not be interrupted*/
                    le_audio_log("[CAP][stream] Modify source FAIL, call is active", 0);
                    ble_bap_bass_modify_source_response(ind->connect_handle, ind->source_id, false);
                    break;
                }

                /*Inform APP to disable sync feature because BSA is used*/
                bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BASS_ADD_SOURCE, p_msg);

                if (ind->param->pa_sync == BT_BASS_PA_SYNC_SYNCHRONIZE_TO_PA_PAST_AVAILABLE) {
                    g_default_bmr_scan_info.audio_channel_allocation = ble_pacs_get_audio_location(AUDIO_DIRECTION_SINK);
                    ble_bap_bass_modify_source_response(ind->connect_handle, ind->source_id, true);
                } else if (ind->param->pa_sync == BT_BASS_PA_SYNC_SYNCHRONIZE_TO_PA_PAST_NOT_AVAILABLE) {
                    bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
                    scan_param.audio_channel_allocation = ble_pacs_get_audio_location(AUDIO_DIRECTION_SINK);
                    scan_param.bms_address = &ind->param->advertiser_addr;
                    scan_param.duration = DEFAULT_SCAN_TIMEOUT;
                    bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
                }
            }
            break;
        }
        case BLE_BAP_BASE_SCAN_STOPPPED_IND:
            bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_SCAN_STOPPED, p_msg);
            break;

        case BLE_BAP_SETUP_ISO_DATA_PATH_NOTIFY: {
            break;
        }

        default:
            break;
    }

    return;
}

static void bt_sink_srv_cap_stream_parameter_init(uint8_t max_link_num)
{
#ifdef AIR_LE_AUDIO_CIS_ENABLE
    uint8_t link_index, ase_index;

    g_cap_stream_service_info.service_connect_handle = BT_HANDLE_INVALID;
    g_cap_stream_service_info.le_handle_with_cis_established = BT_HANDLE_INVALID;
    g_cap_stream_service_info.handle_to_disconnect = BT_HANDLE_INVALID;
    g_cap_stream_service_info.max_link_num = max_link_num;

    for (link_index = 0; link_index < max_link_num; link_index++) {
        for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
            gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id = CAP_INVALID_UINT8;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle = BT_HANDLE_INVALID;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].codec_specific_configuration = NULL;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].codec_specific_configuration_length = 0;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].metadata = NULL;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].metadata_length = 0;
        }
    }

    for (uint8_t i = 0; i < CAP_MAX_CIS_NUM; i++) {
        g_ase_streaming_timer_data[i].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[i].ase_id = 0;
    }
#endif
    g_cap_stream_service_info.big_info.sync_handle = BT_HANDLE_INVALID;
    g_cap_stream_service_info.big_info.big_handle = 0;
    g_default_bmr_scan_info.specified_bms = false;
    g_default_bmr_scan_info.audio_channel_allocation = AUDIO_LOCATION_FRONT_LEFT;
    memset(g_default_bmr_scan_info.bms_address, 0, sizeof(bt_bd_addr_t));
}

#ifdef AIR_LE_AUDIO_CIS_ENABLE
static bool bt_sink_srv_cap_stream_set_ase_link(bt_handle_t connect_handle, uint8_t ase_id)
{
    uint8_t ase_index = 0, link_index = 0;

    //MTK_COMMON:rm this dummy log
    //le_audio_log("[CAP][stream] set ase link, connect handle:%d, ase id:%d", 2, connect_handle, ase_id);
    if (bt_sink_srv_cap_stream_is_ase_link_set(connect_handle, ase_id)) {
        return false;
    }

    if (ase_id != CAP_INVALID_UINT8 && (link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
            if (gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id == CAP_INVALID_UINT8) {
                //MTK_COMMON:change log format due to rm above log
                //le_audio_log("[CAP][stream] ok, ase_id:%d (idx = %d), connHdl:0x%x (idx = %d)",
                //             4, ase_id, ase_index, connect_handle, link_index);
                gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id = ase_id;
                gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_state = ASE_STATE_IDLE;
                gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state = CAP_INVALID_UINT8;
                gp_cap_stream_link_info[link_index].ase_info[ase_index].next_sub_state = CAP_INVALID_UINT8;
                gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle = BT_HANDLE_INVALID;
                gp_cap_stream_link_info[link_index].ase_info[ase_index].iso_interval = 0;
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].codec_specific_configuration) {
                    vPortFree(gp_cap_stream_link_info[link_index].ase_info[ase_index].codec_specific_configuration);
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].codec_specific_configuration = NULL;
                }
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].metadata) {
                    vPortFree(gp_cap_stream_link_info[link_index].ase_info[ase_index].metadata);
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].metadata = NULL;
                }
                memset(&gp_cap_stream_link_info[link_index].ase_info[ase_index].codec[0], CAP_INVALID_UINT8, AUDIO_CODEC_ID_SIZE);
                gp_cap_stream_link_info[link_index].ase_info[ase_index].direction = CAP_INVALID_UINT8;
                return true;
            }
        }
    }

    //MTK_COMMON:change log format due to rm above log
    le_audio_log("[CAP][stream] Fail, ase_id:%d (idx = %d), connHdl:0x%x (idx = %d)",
                  4, ase_id, ase_index, connect_handle, link_index);
    return false;
}

static bool bt_sink_srv_cap_stream_is_ase_link_set(bt_handle_t connect_handle, uint8_t ase_id)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if (ase_id == CAP_INVALID_UINT8) {
        le_audio_log("[CAP][stream] is ase link set, FAIL, wrong ase id", 0);
        return false;
    }

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL
        && p_ase_info->ase_id == ase_id) {
        //le_audio_log("[CAP][stream] is ase link set, SUCCESS", 0);
        return true;
    }

    //(MTK_Titan:shrink log)le_audio_log("[CAP][stream] is ase link set, FAIL, ase link:%d, ase id1:%d, ase id2:%d", 3, (p_ase_info != (bt_sink_srv_cap_stream_ase_info_t *)NULL), p_ase_info ? p_ase_info->ase_id : CAP_INVALID_UINT8, ase_id);
    return false;
}

static bt_sink_srv_cap_stream_ase_info_t *bt_sink_srv_cap_stream_get_ase_link(bt_handle_t connect_handle, uint8_t ase_id)
{
    uint8_t link_index = 0, ase_index = 0;

    if (ase_id != CAP_INVALID_UINT8 && (link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
            if (gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id == ase_id) {
                //(MTK_COMMON: rm log to reduce log)
                //le_audio_log("[CAP][stream] get ase link, SUCCESS, link index:%d", 1, link_index);
                return &gp_cap_stream_link_info[link_index].ase_info[ase_index];
            }
        }
    }

    //(MTK_COMMON: rm log to reduce log)
    //le_audio_log("[CAP][stream] get ase link, FAIL, ase id:%d, link index:%d, ase index:%d", 3, ase_id, link_index, ase_index);
    return (bt_sink_srv_cap_stream_ase_info_t *)NULL;
}

static bt_sink_srv_cap_stream_ase_info_t *bt_sink_srv_cap_stream_get_ase_link_by_cis_handle(bt_handle_t cis_handle)
{
    uint8_t link_index, ase_index;

    if (BT_HANDLE_INVALID != cis_handle) {
        for (link_index = 0; link_index < g_cap_stream_service_info.max_link_num; link_index++) {
            for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle == cis_handle) {
                    //MTK_COMMON: David - shrink log
                    le_audio_log("[CAP][stream] get ase link by cis handle SUCCESS, cis hd:%x, ase id:%d, link index:%d, ase index:%d",
                                 3, cis_handle, gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id, link_index, ase_index);
                    return &gp_cap_stream_link_info[link_index].ase_info[ase_index];
                }
            }
        }
    }
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] get ase link by cis handle FAIL, cis_hd:%x", 1, cis_handle);
    return (bt_sink_srv_cap_stream_ase_info_t *)NULL;
}

/*static bool bt_sink_srv_cap_stream_clear_ase_link(bt_handle_t connect_handle, uint8_t ase_id)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if(ase_id != CAP_INVALID_UINT8 && (p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL)
    {
        le_audio_log("[CAP][stream] clear ase link, SUCCESS", 0);
        p_ase_info->ase_id = CAP_INVALID_UINT8;
        return true;
    }

    le_audio_log("[CAP][stream] clear ase link, FAIL, ase id:%d, ase link:%d", 2, ase_id, (p_ase_info != (bt_sink_srv_cap_stream_ase_info_t *)NULL));
    return false;
}*/

static bool bt_sink_srv_cap_stream_state_check(uint8_t current_state, uint8_t next_state)
{
    bool is_check_ok = false;

    switch (current_state) {
        case ASE_STATE_IDLE:
            if (next_state == ASE_STATE_CODEC_CONFIGURED) {
                is_check_ok = true;
            }
            break;

        case ASE_STATE_CODEC_CONFIGURED:
            if (next_state == ASE_STATE_QOS_CONFIGURED || next_state == ASE_STATE_CODEC_CONFIGURED || next_state == ASE_STATE_RELEASING) {
                is_check_ok = true;
            }
            break;
        case ASE_STATE_QOS_CONFIGURED:
            if (next_state == ASE_STATE_ENABLING || next_state == ASE_STATE_RELEASING || next_state == ASE_STATE_QOS_CONFIGURED || next_state == ASE_STATE_CODEC_CONFIGURED) {
                is_check_ok = true;
            }
            break;
        case ASE_STATE_ENABLING:
            if (next_state == ASE_STATE_STREAMING || next_state == ASE_STATE_DISABLING || next_state == ASE_STATE_RELEASING || next_state == ASE_STATE_CODEC_CONFIGURED || next_state == ASE_STATE_QOS_CONFIGURED) {
                is_check_ok = true;
            }
            break;
        case ASE_STATE_STREAMING:
            if (next_state == ASE_STATE_DISABLING || next_state == ASE_STATE_RELEASING || next_state == ASE_STATE_CODEC_CONFIGURED || next_state == ASE_STATE_QOS_CONFIGURED) {
                is_check_ok = true;
            }
            break;
        case ASE_STATE_DISABLING:
            if (next_state == ASE_STATE_QOS_CONFIGURED || next_state == ASE_STATE_RELEASING || next_state == ASE_STATE_CODEC_CONFIGURED) {
                is_check_ok = true;
            }
            break;
        case ASE_STATE_RELEASING:
            if (next_state == ASE_STATE_CODEC_CONFIGURED || next_state == ASE_STATE_IDLE) {
                is_check_ok = true;
            }
            break;

        default:
            break;
    }

    return is_check_ok;
}

static bool bt_sink_srv_cap_stream_sub_state_check(uint8_t current_sub_state, uint8_t current_state)
{
    if ((current_state == ASE_STATE_QOS_CONFIGURED && current_sub_state == SUB_STATE_ENABLING_RESPONDING) ||
        (current_state == ASE_STATE_ENABLING && current_sub_state == SUB_STATE_ENABLING_RESPONDING) ||
        (current_state == ASE_STATE_ENABLING && current_sub_state == SUB_STATE_ENABLING_NOTIFYING) ||
        ((current_state == ASE_STATE_DISABLING || current_state == ASE_STATE_QOS_CONFIGURED) && current_sub_state == SUB_STATE_DISABLING_RESPONDING) ||
        (current_state == ASE_STATE_RELEASING && (current_sub_state == SUB_STATE_RELEASING_RESPONDING || current_sub_state == SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING)) ||
        (current_state == ASE_STATE_STREAMING && current_sub_state == SUB_STATE_SETTING_DATA_PATH) ||
        ((current_state == ASE_STATE_IDLE || current_state == ASE_STATE_CODEC_CONFIGURED || current_state == ASE_STATE_QOS_CONFIGURED) &&
         (current_sub_state == SUB_STATE_QOS_CONFIG_RESPONDING || current_sub_state == SUB_STATE_CODEC_CONFIG_RESPONDING))) {
        return true;
    }

    return false;
}

/*static bt_sink_srv_cap_am_mode bt_sink_srv_cap_stream_get_mode_by_connection_handle(bt_handle_t connect_handle)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_current_mode();
    uint8_t link_index = bt_sink_srv_cap_get_link_index(connect_handle);
    if (mode % CAP_UNICAST_DEVICE_NUM == link_index) {
        return mode;
    }
    return CAP_INVALID_UINT8;
}*/

static bool bt_sink_srv_cap_stream_set_ase_state(bt_handle_t connect_handle, uint8_t ase_id, uint8_t state)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info = NULL;
    uint8_t pre_state;
    //bt_cap_bap_state_notify_t notify;
    //notify.connect_handle = connect_handle;

    if (state <= ASE_STATE_RELEASING && (p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL
        && bt_sink_srv_cap_stream_state_check(p_ase_info->ase_state, state)) {
        le_audio_log("[CAP][stream] set ase_%d state:%d (cur:%d)", 3, ase_id, state, p_ase_info->ase_state); //MTK_Titan: shink log 
        //notify.pre_state = p_ase_info->ase_state;
        //notify.current_state = state;
        pre_state = p_ase_info->ase_state;
        p_ase_info->ase_state = state;

        /*if(bt_sink_srv_cap_inform_app(CAP_ASE_STATE_NOTIFY, &notify))
        {
            return true;
        }*/

        if (ASE_STATE_STREAMING == state) {
#if 1 //MTK_COMMON: To support reason code update
            bt_sink_srv_cap_set_state(connect_handle, BT_SINK_SRV_CAP_STATE_ASE_STREAMING, BT_HCI_STATUS_SUCCESS);
#else
            bt_sink_srv_cap_set_state(connect_handle, BT_SINK_SRV_CAP_STATE_ASE_STREAMING);
#endif
        } else if (ASE_STATE_STREAMING == pre_state && ASE_STATE_STREAMING != state) {
#if 1 //MTK_COMMON: To support reason code update
            bt_sink_srv_cap_set_state(connect_handle, BT_SINK_SRV_CAP_STATE_CONNECTED, BT_HCI_STATUS_SUCCESS);
#else
            bt_sink_srv_cap_set_state(connect_handle, BT_SINK_SRV_CAP_STATE_CONNECTED);
#endif
        }
        return true;
    }

    le_audio_log("[CAP][stream] set ase state, FAIL, p_ase_info:0x%x, ase_id:%d, current state:%d, state:%d", 4, p_ase_info, ase_id, p_ase_info ? p_ase_info->ase_state : CAP_INVALID_UINT8, state);
    return false;
}

static bool bt_sink_srv_cap_stream_set_ase_sub_state(bt_handle_t connect_handle, uint8_t ase_id, uint8_t sub_state)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL
        && (p_ase_info->sub_state == CAP_INVALID_UINT8 || (sub_state == CAP_INVALID_UINT8 && bt_sink_srv_cap_stream_sub_state_check(p_ase_info->sub_state, p_ase_info->ase_state)) ||
            (p_ase_info->sub_state == SUB_STATE_ENABLING_RESPONDING && sub_state == SUB_STATE_ENABLING_NOTIFYING) ||
            (p_ase_info->sub_state == SUB_STATE_QOS_CONFIG_RESPONDING && sub_state == SUB_STATE_RELEASING_RESPONDING))) {
        le_audio_log("[CAP][stream] set ase_%d sub_state:%d", 2, ase_id, sub_state); //MTK_Titan: shink log
        p_ase_info->sub_state = sub_state;
        return true;
    }

    le_audio_log("[CAP][stream] set ase sub state, FAIL, p_ase_info:0x%x, ase id:%d, current sub state:%d, sub state:%d", 4, p_ase_info, ase_id, p_ase_info ? p_ase_info->sub_state : CAP_INVALID_UINT8, sub_state);
    return false;
}

static bool bt_sink_srv_cap_stream_set_ase_next_sub_state(bt_handle_t connect_handle, uint8_t ase_id, uint8_t next_sub_state)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {

        le_audio_log("[CAP][stream] set next sub state, ase_id:%d, cur_sub_state:%d, next_sub_state:%d)", 3, ase_id, p_ase_info->sub_state, next_sub_state);

        if (SUB_STATE_DISABLING_RESPONDING == p_ase_info->sub_state || SUB_STATE_ENABLING_RESPONDING == p_ase_info->sub_state || SUB_STATE_ENABLING_NOTIFYING == p_ase_info->sub_state) {
            if (next_sub_state == SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING || next_sub_state == SUB_STATE_QOS_CONFIG_RESPONDING) {
                p_ase_info->next_sub_state = next_sub_state;
                return true;
            }
        }
    }
    return false;
}

static uint8_t bt_sink_srv_cap_stream_get_ase_state(bt_handle_t connect_handle, uint8_t ase_id)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        return p_ase_info->ase_state;
    }

    //(MTK_Titan: shrink log)le_audio_log("[CAP][stream] get ase state, FAIL", 0);
    return CAP_INVALID_UINT8;
}

static uint8_t bt_sink_srv_cap_stream_get_ase_sub_state(bt_handle_t connect_handle, uint8_t ase_id)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        //(MTK_Titan: shrink log)le_audio_log("[CAP][stream] get ase sub state: %d",1 , p_ase_info->sub_state);
        return p_ase_info->sub_state;
    }

    //(MTK_Titan: shrink log)le_audio_log("[CAP][stream] get ase sub state, FAIL", 0);
    return CAP_INVALID_UINT8;
}

static bool bt_sink_srv_cap_stream_check_and_update_ase_next_sub_state(bt_handle_t connect_handle, uint8_t ase_id)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        le_audio_log("[CAP][stream] update next sub state, ase_id:%d, state:%d cur_sub_state:%d, next_sub_state:%d", 4,
                     ase_id, p_ase_info->ase_state, p_ase_info->sub_state, p_ase_info->next_sub_state);

        if ((ASE_STATE_QOS_CONFIGURED == p_ase_info->ase_state && AUDIO_DIRECTION_SINK == p_ase_info->direction) ||
            (ASE_STATE_DISABLING == p_ase_info->ase_state && AUDIO_DIRECTION_SOURCE == p_ase_info->direction) ||
            (ASE_STATE_ENABLING == p_ase_info->ase_state)) {
            if (p_ase_info->next_sub_state == SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING) {
                bt_sink_srv_cap_stream_release_autonomously(connect_handle, ase_id, false, 0);
                p_ase_info->next_sub_state = CAP_INVALID_UINT8;
                return true;
            } else if (p_ase_info->next_sub_state == SUB_STATE_QOS_CONFIG_RESPONDING) {
                ble_bap_ase_qos_config_response(connect_handle, ase_id, RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
                p_ase_info->next_sub_state = CAP_INVALID_UINT8;
                return true;
            }
        }
    }
    return false;
}

static bool bt_sink_srv_cap_stream_check_ase_streaming_and_processing(bt_sink_srv_cap_stream_ase_info_t *ase_info, bool is_processing)
{
    //le_audio_log("[CAP][stream] bt_sink_srv_cap_stream_check_ase_streaming_and_processing[%d]", 1, is_processing);
    if(ase_info != NULL) {
        if (!is_processing) {
            if(ase_info->ase_state == ASE_STATE_STREAMING ||
                ase_info->ase_state == ASE_STATE_ENABLING ||
                ase_info->sub_state == SUB_STATE_ENABLING_NOTIFYING) {
                return true;
            }
        } else {
            if((ase_info->ase_state == ASE_STATE_STREAMING ||
                ase_info->ase_state == ASE_STATE_ENABLING ||
                ase_info->sub_state == SUB_STATE_ENABLING_NOTIFYING ||
                ase_info->sub_state == SUB_STATE_ENABLING_RESPONDING) &&
                (ase_info->sub_state < SUB_STATE_DISABLING_RESPONDING ||
                ase_info->sub_state == CAP_INVALID_UINT8)) {
                return true;
            }
        }
    }
    return false;
}

static uint8_t bt_sink_srv_cap_stream_get_ase_direction(bt_handle_t connect_handle, uint8_t ase_id)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        return p_ase_info->direction;
    }

    le_audio_log("[CAP][stream] get ase direction, FAIL", 0);
    return CAP_INVALID_UINT8;
}

/*static uint8_t bt_sink_srv_cap_stream_get_ase_id_by_direction(bt_handle_t connect_handle, uint8_t direction)
{
    uint8_t link_index = 0, ase_index;

    if((direction == AUDIO_DIRECTION_SINK || direction == AUDIO_DIRECTION_SOURCE) &&
       (link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num)
    {
        for(ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++)
        {
            if(gp_cap_stream_link_info[link_index].ase_info[ase_index].direction == direction)
            {
                return gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
            }
        }
    }

    le_audio_log("[CAP][stream] get ase id by direction, FAIL, direction:%d, link index:%d, ase index:%d", 3, direction, link_index, ase_index);
    return CAP_INVALID_UINT8;
}*/

static ble_bap_ase_id_list_t bt_sink_srv_cap_stream_find_ase_id_list_with_cis_established(bt_handle_t connect_handle)
{
    uint8_t link_index = 0, ase_index;
    ble_bap_ase_id_list_t ase_id_list = {0};

    if ((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
            if (gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle &&
                gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle != BT_HANDLE_INVALID) {
                ase_id_list.ase_id[ase_id_list.num_of_ase] = gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
                ase_id_list.num_of_ase++;
            }
        }
    }
    le_audio_log("[CAP][stream] bt_sink_srv_cap_stream_find_ase_id_list_with_cis [num:%d], [ase_id: %x %x %x]", 4,
                 ase_id_list.num_of_ase, ase_id_list.ase_id[0], ase_id_list.ase_id[1], ase_id_list.ase_id[2]);
    return ase_id_list;
}

static uint8_t bt_sink_srv_cap_stream_get_cis_list(bt_handle_t connect_handle, bt_handle_t *cis_list)
{
    uint8_t cis_num = 0, link_index = 0, ase_index;
    bt_handle_t check_cis = BT_HANDLE_INVALID;

    if ((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
            if (check_cis != gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle &&
                gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle != BT_HANDLE_INVALID) {

                cis_list[cis_num] = gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle;
                check_cis = cis_list[cis_num];
                cis_num++;
            }
            if (cis_num >= MAX_CIS_NUM) {
                break;
            }
        }
    }
    le_audio_log("[CAP][stream] bt_sink_srv_cap_stream_get_cis_list [num:%d], [cis: %x, %x]", 3,
                 cis_num, cis_list[0], cis_list[1]);
    return cis_num;
}

/*static uint8_t bt_sink_srv_cap_stream_get_uplink_cis_list(bt_handle_t connect_handle, bt_handle_t *cis_list)
{
    uint8_t cis_num = 0, link_index = 0, ase_index;
    bt_handle_t check_cis = BT_HANDLE_INVALID;

    if ((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {

            if (check_cis != gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle &&
                gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle != BT_HANDLE_INVALID &&
                gp_cap_stream_link_info[link_index].ase_info[ase_index].direction == AUDIO_DIRECTION_SOURCE) {

                cis_list[cis_num] = gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle;
                check_cis = cis_list[cis_num];
                cis_num++;
            }
            if (cis_num >= MAX_CIS_NUM) {
                break;
            }
        }
    }
    le_audio_log("[CAP][stream] bt_sink_srv_cap_stream_get_uplink_cis_list [num:%d], [cis: %x, %x]", 3,
        cis_num, cis_list[0], cis_list[1]);
    return cis_num;
}*/

/*static bool bt_sink_srv_cap_stream_clear_service_big(void)
{
    bt_sink_srv_cap_stream_service_big_t service_big = bt_sink_srv_cap_stream_get_service_big();
    le_audio_log("[CAP][stream] clear service big, sync_handle:%04x, num_bis:%d", service_big.sync_handle, service_big.num_bis);

    if((service_big.sync_handle == BT_HANDLE_INVALID || service_big.sync_handle == 0 || service_big.big_handle == 0) &&
        !g_cap_stream_service_info.big_info.num_bis && !g_cap_stream_service_info.big_info.bis_indices)
    {
        return false;
    }

    g_cap_stream_service_info.big_info.sync_handle = BT_HANDLE_INVALID;
    g_cap_stream_service_info.big_info.big_handle = 0;
    g_cap_stream_service_info.big_info.num_bis = 0;

    if(g_cap_stream_service_info.big_info.bis_indices != NULL)
    {
        vPortFree(g_cap_stream_service_info.big_info.bis_indices);
        g_cap_stream_service_info.big_info.bis_indices = NULL;
    }

    return true;
}*/

static bool bt_sink_srv_cap_stream_clear_cis_handle(bt_handle_t connect_handle, uint8_t ase_id)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        le_audio_log("[CAP][stream] clear cis handle, SUCCESS", 0);

        bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
        uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(connect_handle, cis_list);

        p_ase_info->cis_handle = BT_HANDLE_INVALID;
        p_ase_info->iso_interval = 0;
        if (g_cap_stream_service_info.le_handle_with_cis_established != BT_HANDLE_INVALID && cis_num == 1) {
            /*the last CIS*/
            g_cap_stream_service_info.le_handle_with_cis_established = BT_HANDLE_INVALID;
        }
        return true;
    }

    le_audio_log("[CAP][stream] clear cis handle, FAIL", 0);
    return false;
}

static bool bt_sink_srv_cap_stream_is_ble_link_serviced(bt_handle_t connect_handle)
{
    if (connect_handle == BT_HANDLE_INVALID || bt_sink_srv_cap_stream_get_service_ble_link() != connect_handle) {
        return false;
    }

    return true;
}

bool bt_sink_srv_cap_stream_set_service_ble_link(bt_handle_t connect_handle)
{
    le_audio_log("[CAP][stream] set service ble link, connect handle:%d", connect_handle);
    if (connect_handle == BT_HANDLE_INVALID) {
        return false;
    }

    g_cap_stream_service_info.service_connect_handle = connect_handle;

    return true;
}

static bool bt_sink_srv_cap_stream_set_codec(ble_bap_ase_codec_config_ind_t *p_msg)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info = NULL;

    if (p_msg->codec_config->param->codec_id[0] != CAP_INVALID_UINT8 && p_msg->codec_config->direction != CAP_INVALID_UINT8
        && (p_ase_info = bt_sink_srv_cap_stream_get_ase_link(p_msg->connect_handle, p_msg->codec_config->param->ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        if (p_ase_info->ase_state == ASE_STATE_IDLE || p_ase_info->ase_state == ASE_STATE_CODEC_CONFIGURED || p_ase_info->ase_state == ASE_STATE_QOS_CONFIGURED) {
            p_ase_info->direction = p_msg->codec_config->direction;
            p_ase_info->target_latency = p_msg->codec_config->param->target_latency;
            //le_audio_log("[CAP][stream] set target_latency:%d, phy:%d", 2, p_msg->target_latency, p_msg->target_phy);
            memcpy(&p_ase_info->codec[0], &p_msg->codec_config->param->codec_id[0], AUDIO_CODEC_ID_SIZE);
            p_ase_info->codec_specific_configuration_length = p_msg->codec_config->param->codec_specific_configuration_length;

            if (NULL != p_ase_info->codec_specific_configuration) {
                vPortFree(p_ase_info->codec_specific_configuration);
                p_ase_info->codec_specific_configuration = NULL;
            }

            if ((p_ase_info->codec_specific_configuration = pvPortMalloc(p_ase_info->codec_specific_configuration_length)) == NULL) {
                configASSERT(0);
            } else {
                memcpy(&p_ase_info->codec_specific_configuration[0], &p_msg->codec_config->param->codec_specific_configuration[0], p_msg->codec_config->param->codec_specific_configuration_length);
            }
            le_audio_log("[CAP][stream] set codec, SUCCESS", 0);
            return true;
        }
    }

    le_audio_log("[CAP][stream] set codec, FAIL, codec:%d, direction:%d, ase link:%d", 3,
                 p_msg->codec_config->param->codec_id[0], p_msg->codec_config->direction, (p_ase_info != (bt_sink_srv_cap_stream_ase_info_t *)NULL));
    return false;
}

static bool bt_sink_srv_cap_stream_set_qos(ble_bap_ase_qos_config_ind_t *p_msg)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info = NULL;

    if (p_msg != (ble_bap_ase_qos_config_ind_t *)NULL && (p_ase_info = bt_sink_srv_cap_stream_get_ase_link(p_msg->connect_handle, p_msg->qos_config->ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        le_audio_log("[CAP][stream] set qos, SUCCESS", 0);
        memcpy(&p_ase_info->cig_id, &p_msg->qos_config->cig_id, sizeof(ble_ascs_config_qos_operation_t) - 1);
        le_audio_log("CAP QOS frame_payload_size:%4x", 1, p_ase_info->maximum_sdu_size);
        return true;
    }

    le_audio_log("[CAP][stream] set qos, FAIL, qos:%d, ase link:%d", 2, (p_msg != (ble_bap_ase_qos_config_ind_t *)NULL), (p_ase_info != (bt_sink_srv_cap_stream_ase_info_t *)NULL));
    return false;
}

static bool bt_sink_srv_cap_stream_set_metadata(ble_bap_ase_enable_ind_t *p_msg)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info = NULL;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(p_msg->connect_handle, p_msg->ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        p_ase_info->metadata_length = p_msg->metadata_length;

        //MTK_COMMON: David - reduce log
        //le_audio_log("[CAP][stream] set metadata, p_metadata:0x%x, set length:%d", 2, p_ase_info->metadata, p_ase_info->metadata_length);

        if (NULL != p_ase_info->metadata) {
            vPortFree(p_ase_info->metadata);
            p_ase_info->metadata = NULL;
        }

        if (0 != p_ase_info->metadata_length) {
            if ((p_ase_info->metadata = pvPortMalloc(p_ase_info->metadata_length)) == NULL) {
                configASSERT(0);
                return false;
            } else {
                memcpy(&p_ase_info->metadata[0], &p_msg->metadata[0], p_msg->metadata_length);
            }
        }

        //MTK_COMMON: David - reduce log
        //le_audio_log("[CAP][stream] set metadata, SUCCESS, ase_id:%d, len:%d", 2, p_msg->ase_id, p_msg->metadata_length);
        return true;
    }

    le_audio_log("[CAP][stream] set metadata, FAIL,  ase_id:%d, len:%d", 2, p_msg->ase_id, p_msg->metadata_length);
    return false;
}

static bool bt_sink_srv_cap_stream_set_cis_handle(bt_handle_t connect_handle, uint8_t ase_id, bt_handle_t cis_handle)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info = NULL;

    if (cis_handle != BT_HANDLE_INVALID && (p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        //MTK_COMMON: David - reduce log
        //le_audio_log("[CAP][stream] set cis handle, SUCCESS", 0);
        p_ase_info->cis_handle = cis_handle;
        g_cap_stream_service_info.le_handle_with_cis_established = connect_handle;
        return true;
    }

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] set cis handle, FAIL, cis hd:%d, ase link:%d", 2, (cis_handle != BT_HANDLE_INVALID), (p_ase_info != (bt_sink_srv_cap_stream_ase_info_t *)NULL));
    return false;
}

#ifdef MTK_LE_CAP_STREAM_COMMON
static void bt_sink_srv_cap_stream_check_cis_channel_info(bt_handle_t connect_handle)
{
    uint8_t i;
    uint8_t max_num_of_ase = ble_ascs_get_ase_number();
    uint8_t target_num_of_ase = ble_ascs_get_num_of_ase(connect_handle);
    uint8_t actual_num_of_ase = 0;
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info = NULL;
    uint8_t *p_channel_info = NULL;
    uint8_t channel_info = 0;
    bt_handle_t cis_handle_L = 0;
    bt_handle_t cis_handle_R = 0;

    le_audio_log("[CAP][stream] connect handle:0x%x, ASE(%d, %d)", 3, connect_handle, target_num_of_ase, max_num_of_ase);

    if (!target_num_of_ase)
        return;

    for (i = 0; i < max_num_of_ase, actual_num_of_ase < target_num_of_ase; i++) {
        p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, i);

        /* If one of the CIS did not ready yet, skip this ASE */
        if (!p_ase_info || p_ase_info->cis_handle == BT_HANDLE_INVALID || p_ase_info->direction != AUDIO_DIRECTION_SINK)
            continue;

        actual_num_of_ase++;

        p_channel_info = ble_ascs_get_ltv_value_from_codec_specific_configuration(CODEC_CONFIGURATION_TYPE_AUDIO_CHANNEL_ALLOCATION,
                        p_ase_info->codec_specific_configuration_length, p_ase_info->codec_specific_configuration);
        if (p_channel_info == NULL) {
            le_audio_log("[CAP][stream] Channel info not exist! Set Chennel Left as default!", 0);
            channel_info = AUDIO_LOCATION_FRONT_LEFT;
        } else {
            channel_info = *p_channel_info;
            if (!(channel_info & (AUDIO_LOCATION_FRONT_LEFT | AUDIO_LOCATION_FRONT_RIGHT))) {
                le_audio_log("[CAP][stream] Not supported Channel setting 0x%x", 1, channel_info);
                continue;
            }
        }

        if (channel_info & AUDIO_LOCATION_FRONT_LEFT) {
            /* Handle the case that the first CIS did not provide channel info but the second CIS provide the channel info as L channel. */
            if (cis_handle_L != 0 && cis_handle_L != p_ase_info->cis_handle)
                cis_handle_R = cis_handle_L;

            cis_handle_L = p_ase_info->cis_handle;
        }
        if (channel_info & AUDIO_LOCATION_FRONT_RIGHT)
            cis_handle_R = p_ase_info->cis_handle;
    }

    if (actual_num_of_ase != target_num_of_ase) {
        le_audio_log("[CAP][stream] CIS not ready expect %d actual %d", 2, target_num_of_ase, actual_num_of_ase);
        return;
    }

    bt_sink_cap_am_set_cis_handle(cis_handle_L, cis_handle_R);
}
#endif

static bt_status_t bt_sink_srv_cap_stream_set_cis_data_path_by_cis_handle(bt_handle_t connect_handle, bt_handle_t cis_handle, ble_bap_ase_id_list_t ase_id_list)
{
    uint8_t i = 0;
    uint8_t specified_ase = 0,  source_ase = 0;
    bt_sink_srv_cap_stream_ase_info_t *ase_info = NULL;

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] Set cis data path, conn_hd:%x, cis_hd:%x", 2, connect_handle, cis_handle);

    ble_bap_ase_id_list_t streaming_ase_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(connect_handle, false);
    if (!streaming_ase_list.num_of_ase) {
        /*No ASE is enabling/streaming state, no need to set ISO data path*/
        return BT_STATUS_FAIL;
    }

    /*for (i = 0; i < ase_id_list.num_of_ase; i++) {
        state = bt_sink_srv_cap_stream_get_ase_state(connect_handle, ase_id_list.ase_id[i]);
        if (ASE_STATE_QOS_CONFIGURED != state || ASE_STATE_ENABLING != state || ASE_STATE_STREAMING != state) {
            return BT_STATUS_FAIL;
        }
    }*/

#if 1
    /*MTK_Titan: (1)To prevent wrong using of data_path_id, so we alloc 2 bytes.
                 (2)In share memory mode, we should use id = 2, which is a vendor ID defined by controller*/
    uint8_t data_path_id[2] = {2, 2};//{2, 2};
    return ble_bap_set_cis_data_path(1, &cis_handle, &data_path_id[0]);
#else
    uint8_t data_path_id = 1;

    if (le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET) {

        for (i = 0; i < streaming_ase_list.num_of_ase; i++) {
            ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, streaming_ase_list.ase_id[i]);
            if (ase_info != NULL && ase_info->cis_handle == cis_handle) {
                specified_ase = streaming_ase_list.ase_id[i];
            }
            if (AUDIO_DIRECTION_SOURCE == bt_sink_srv_cap_stream_get_ase_direction(connect_handle, streaming_ase_list.ase_id[i])) {
                source_ase = streaming_ase_list.ase_id[i];
            }
        }

        le_audio_log("[CAP][stream] specified_ase:%d, source_ase:%d", 2, specified_ase, source_ase);

        if (source_ase) {
            /*Call mode*/
            if (specified_ase == source_ase) {
                /* Headset Uplink CIS must set data_path_id = 1 */
                data_path_id = 1;
            } else {
                /*Pure downlink CIS in call mode*/
                data_path_id = 2;
            }
        } else if (streaming_ase_list.num_of_ase == 2) {
            /* 2 mono CIS music mode*/
            data_path_id = (specified_ase == streaming_ase_list.ase_id[0] ? 1 : 2);
        } else {
            /* 1 stereo CIS music mode*/
            data_path_id = 1;
        }
    } else {
        /* Earbud always sets data_path_id = 1 */
        data_path_id = 1;
    }
    return ble_bap_set_cis_data_path(1, &cis_handle, &data_path_id);
#endif /* #if 1*/

}


static bt_handle_t bt_sink_srv_cap_stream_get_cis_handle(bt_handle_t connect_handle, uint8_t ase_id)
{
    bt_sink_srv_cap_stream_ase_info_t *p_ase_info;

    if ((p_ase_info = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
        return p_ase_info->cis_handle;
    }

    return BT_HANDLE_INVALID;
}

static void bt_sink_srv_cap_stream_state_notify_handler(ble_bap_ase_state_notify_t *p_msg)
{
    uint8_t direction = bt_sink_srv_cap_stream_get_ase_direction(p_msg->connect_handle, p_msg->ase_id);
    uint8_t sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(p_msg->connect_handle, p_msg->ase_id);
    bt_handle_t cis_handle = bt_sink_srv_cap_stream_get_cis_handle(p_msg->connect_handle, p_msg->ase_id);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] state notify handler, conn_hd:%d, ase id:%d, state:%d, dir:%d, cis_hd:%x", 5,
                 p_msg->connect_handle, p_msg->ase_id, p_msg->ase_state, direction, cis_handle);

    if (bt_sink_srv_cap_stream_set_ase_state(p_msg->connect_handle, p_msg->ase_id, p_msg->ase_state) &&
        bt_sink_srv_cap_stream_set_ase_sub_state(p_msg->connect_handle, p_msg->ase_id, CAP_INVALID_UINT8)) {
        if (p_msg->ase_state == ASE_STATE_RELEASING) {
            if (sub_state == SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING) {
                /*Server shall terminate CIS connection because it has initiated release operation autonomously before*/
                if (cis_handle != BT_HANDLE_INVALID && cis_handle != 0) {
                    /*ble_bap_disconnect_cis(cis_handle);*/
                    bt_sink_srv_cap_stream_cis_disconnection_timer_start(cis_handle);
                } else {
                    ble_bap_codec_config_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
                }
            } else if (cis_handle == BT_HANDLE_INVALID || cis_handle == 0) {
                ble_bap_codec_config_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
            }
        }

        if (direction == AUDIO_DIRECTION_SINK) {
            if (p_msg->ase_state == ASE_STATE_DISABLING) {
                configASSERT(0);
                //bt_sink_srv_cap_stream_receiver_ready_handler(p_msg->connect_handle, p_msg->ase_id, true);
            }
            if ((p_msg->ase_state == ASE_STATE_ENABLING) && (cis_handle != BT_HANDLE_INVALID && cis_handle != 0)) {
                //If CIS has already established, send receiver start ready notification
                bt_sink_srv_cap_stream_receiver_ready_handler(p_msg->connect_handle, p_msg->ase_id, true);
            }
        }
    }

    bt_sink_srv_cap_stream_check_and_update_ase_next_sub_state(p_msg->connect_handle, p_msg->ase_id);
}

static void bt_sink_srv_cap_stream_codec_config_ind_handler(ble_bap_ase_codec_config_ind_t *p_msg)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] codec config ind handler, conn_hd:%d, ase id:%d, codec id:%d, dir:%d", 4,
                 p_msg->connect_handle, p_msg->codec_config->param->ase_id, p_msg->codec_config->param->codec_id[0], p_msg->codec_config->direction);

    if (!bt_sink_srv_cap_stream_set_codec(p_msg)) {
        configASSERT(0);
    }

    //ble_bap_codec_config_response(p_msg->connect_handle, p_msg->codec_config->ase_id, response_code, ERROR_REASON_NO_ERROR);
}

static void bt_sink_srv_cap_stream_qos_config_ind_handler(ble_bap_ase_qos_config_ind_t *p_msg)
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->qos_config->ase_id);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] qos config ind handler, conn_hd:%d, ase id:%d, state:%d", 3, p_msg->connect_handle, p_msg->qos_config->ase_id, state);
    if ((state != ASE_STATE_CODEC_CONFIGURED && state != ASE_STATE_QOS_CONFIGURED) || !bt_sink_srv_cap_stream_set_qos(p_msg)) {
        configASSERT(0);
    }
    //ble_bap_ase_qos_config_response(p_msg->connect_handle, p_msg->qos_config->ase_id, response_code, ERROR_REASON_NO_ERROR);
}

static void bt_sink_srv_cap_stream_ase_enabling_ind_handler(ble_bap_ase_enable_ind_t *p_msg)
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] ase enabling ind handler, conn_hd:%d, ase id:%d, state:%d", 3, p_msg->connect_handle, p_msg->ase_id, state);

    if (!bt_sink_srv_cap_stream_is_enabling_state_ok(state)) {
        ble_bap_ase_enable_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_INVALID_ASE_STATE_MACHINE_TRANSITION, ERROR_REASON_NO_ERROR);
    } else if (!bt_sink_srv_cap_stream_is_ase_link_set(p_msg->connect_handle, p_msg->ase_id)) {
        ble_bap_ase_enable_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_INSUFFICIENT_RESOURCES, ERROR_REASON_NO_ERROR);
    } else {
        bt_sink_srv_cap_stream_set_metadata(p_msg);
        bt_sink_srv_cap_stream_set_ase_sub_state(p_msg->connect_handle, p_msg->ase_id, SUB_STATE_ENABLING_RESPONDING);
        //bt_sink_srv_cap_stream_push_ase_id(p_msg->connect_handle, p_msg->ase_id);

        if (p_msg->is_last_ase) { //Check start streaming or not
            uint8_t sink_ase_id = bt_sink_srv_cap_stream_pop_proccessing_ase_id(p_msg->connect_handle, AUDIO_DIRECTION_SINK, true);
            uint8_t sink_ase_id_2 = bt_sink_srv_cap_stream_pop_second_proccessing_ase_id(p_msg->connect_handle, AUDIO_DIRECTION_SINK, true);
            uint8_t source_ase_id = bt_sink_srv_cap_stream_pop_proccessing_ase_id(p_msg->connect_handle, AUDIO_DIRECTION_SOURCE, true);

            if ((source_ase_id == p_msg->ase_id && CAP_INVALID_UINT8 == sink_ase_id)) {
                le_audio_log("[CAP][stream] Enable UL Mode Only", 0);
                bt_sink_srv_cap_am_mode mode = (CAP_AM_UNICAST_MUSIC_MODE_START + bt_sink_srv_cap_get_link_index(p_msg->connect_handle));

                bt_sink_srv_cap_am_audio_start(mode);
                bt_sink_srv_cap_set_sub_state(p_msg->connect_handle, BT_SINK_SRV_CAP_SUB_STATE_ASE_MUSIC_ENABLING);
                //ble_bap_ase_enable_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
            } else if ((sink_ase_id == p_msg->ase_id && CAP_INVALID_UINT8 == source_ase_id)) {
                /*Only one ASE is enabled*/
                bt_sink_srv_cap_am_mode mode = (CAP_AM_UNICAST_MUSIC_MODE_START + bt_sink_srv_cap_get_link_index(p_msg->connect_handle));

                le_audio_log("[CAP][stream] Enable DL Mode Only", 0);

                ble_bap_ase_id_list_t ase_id_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(p_msg->connect_handle, false);
                if (le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET && ase_id_list.num_of_ase == 2) {
                    /*2 ASE is already enable in call mode, no need to start audio*/
                    ble_bap_ase_enable_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
                } else {
                    //bt_sink_srv_cap_am_init(UNICAST_MUSIC_MODE);
                    bt_sink_srv_cap_am_audio_start(mode);
                    bt_sink_srv_cap_set_sub_state(p_msg->connect_handle, BT_SINK_SRV_CAP_SUB_STATE_ASE_MUSIC_ENABLING);
                }
            } else if ((sink_ase_id_2 == p_msg->ase_id && CAP_INVALID_UINT8 == source_ase_id)) {
                if (le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET) {
                    le_audio_log("Enable both DL Mode", 0);
                    bt_sink_srv_cap_am_mode mode = (CAP_AM_UNICAST_MUSIC_MODE_START + bt_sink_srv_cap_get_link_index(p_msg->connect_handle));
                    //bt_sink_srv_cap_am_init(UNICAST_MUSIC_MODE);
                    bt_sink_srv_cap_am_audio_start(mode);
                    bt_sink_srv_cap_set_sub_state(p_msg->connect_handle, BT_SINK_SRV_CAP_SUB_STATE_ASE_MUSIC_ENABLING);
                }

            } else if (sink_ase_id == source_ase_id && sink_ase_id != CAP_INVALID_UINT8 && source_ase_id != CAP_INVALID_UINT8) {
                /*Both ASEs use the same direction*/
                if (le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET) {
                    le_audio_log("Enable both DL Mode 2", 0);
                    bt_sink_srv_cap_am_mode mode = (CAP_AM_UNICAST_MUSIC_MODE_START + bt_sink_srv_cap_get_link_index(p_msg->connect_handle));
                    //bt_sink_srv_cap_am_init(UNICAST_MUSIC_MODE);
                    bt_sink_srv_cap_am_audio_start(mode);
                    bt_sink_srv_cap_set_sub_state(p_msg->connect_handle, BT_SINK_SRV_CAP_SUB_STATE_ASE_MUSIC_ENABLING);
                } else {
                    bt_sink_srv_cap_stream_enabling_response(p_msg->connect_handle, false, AUDIO_DIRECTION_SINK);
                    bt_sink_srv_cap_stream_enabling_response(p_msg->connect_handle, false, AUDIO_DIRECTION_SOURCE);
                }
                /*Do twice to respond both ASE Characteristic*/

            } else if (sink_ase_id != source_ase_id && sink_ase_id != CAP_INVALID_UINT8 && source_ase_id != CAP_INVALID_UINT8) {
                bt_sink_srv_cap_am_mode mode = (CAP_AM_UNICAST_CALL_MODE_START + bt_sink_srv_cap_get_link_index(p_msg->connect_handle));

                le_audio_log("Enable DL UL Mode", 0);
#if 1 //To inform APP that bidirectional LE audio streaming state, APP may need this event to apply different DSP algorithms.
                /*Inform upper layer Bidirection LEA is enabling*/
                bt_sink_srv_bidirection_lea_state_update_t update = {0};
                update.le_handle = p_msg->connect_handle;
                update.state = BT_SINK_SRV_BIDIRECTION_LEA_STATE_ENABLE;
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_LE_BIDIRECTION_LEA_UPDATE, &update, sizeof(bt_sink_srv_bidirection_lea_state_update_t));
#endif

                bt_sink_srv_cap_am_audio_start(mode);

                if (sink_ase_id != CAP_INVALID_UINT8 && sink_ase_id_2 != CAP_INVALID_UINT8) {
                    bt_sink_srv_cap_set_sub_state(p_msg->connect_handle, BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_ENABLING_3_ASE);
                } else {
                    bt_sink_srv_cap_set_sub_state(p_msg->connect_handle, BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_ENABLING_2_ASE);
                }

            } else {
                configASSERT(0);
            }

        } else {
            //Wait another ASE being enabled
        }
    }
}

static void bt_sink_srv_cap_stream_cis_requeset_ind_handler(ble_bap_cis_established_ind_t *p_msg)
{
    uint8_t state, sub_state;
    uint8_t is_state_ok = 0;
    for (uint8_t i = 0; i < p_msg->ase_id_list.num_of_ase; i++) {
        state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
        sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
        //MTK_COMMON: David - shrink log
        le_audio_log("[CAP][stream] cis request ind handler, conn_hd:%d, ase id:%d, state:%d", 3,
                     p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], state);

        if ((state != ASE_STATE_ENABLING && state != ASE_STATE_STREAMING && state != ASE_STATE_QOS_CONFIGURED) ||
            (sub_state >= SUB_STATE_DISABLING_RESPONDING && sub_state <= SUB_STATE_QOS_CONFIG_RESPONDING)) {
            ble_bap_cis_request_response(p_msg->cis_handle, false);
            bt_sink_srv_cap_stream_am_timer_stop();
            return;
        } else {
            is_state_ok++;
        }
    }
    if (is_state_ok == p_msg->ase_id_list.num_of_ase) {
        ble_bap_cis_request_response(p_msg->cis_handle, true);
    }
}

static void bt_sink_srv_cap_stream_cis_established_notify_handler(ble_bap_cis_established_notify_t *p_msg)
{
    uint8_t state, sub_state, valid_ase_num = 0;
    bt_handle_t cis_handle;
    uint8_t link_idx = bt_sink_srv_cap_get_link_index(p_msg->connect_handle);
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_current_mode();

    for (uint8_t i = 0; i < p_msg->ase_id_list.num_of_ase; i++) {
        bt_sink_srv_cap_stream_ase_info_t *p_ase_info = bt_sink_srv_cap_stream_get_ase_link(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
        state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
        sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
        cis_handle = bt_sink_srv_cap_stream_get_cis_handle(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
        if (NULL != p_ase_info) {
            p_ase_info->iso_interval = p_msg->iso_interval;
        }

        //MTK_COMMON: David - shrink log
        le_audio_log("[CAP][stream] cis established notify handler, conn_hd:0x%x, ase id:%d, state:%d, cis_hd:0x%x", 4,
            p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], state, p_msg->cis_handle);

        if (state == ASE_STATE_ENABLING || state == ASE_STATE_STREAMING || state == ASE_STATE_QOS_CONFIGURED) {
            valid_ase_num++;
            uint8_t phone_addr[6] = {0};

            bt_sink_srv_cap_stream_set_cis_handle(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], p_msg->cis_handle);

            if (g_cap_stream_am_timer) {
                bt_sink_srv_cap_stream_am_timer_stop();
            }

            if (bt_sink_srv_cap_stream_get_ase_direction(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]) == AUDIO_DIRECTION_SINK) {
                //bt_sink_srv_cap_stream_receiver_ready_handler(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], true);
                bt_sink_srv_cap_stream_send_ase_streaming_state_timer_start(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
            }

            /* exit sniff mode when le audio start streaming */
            //MTK_Titan: Use "BT_CM_PROFILE_SERVICE_MASK_NONE" to fix compiling issue
            if (0 != bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, (bt_bd_addr_t *)phone_addr, 1)) {
            /* Since the bt_gap_exit_sniff_mode only include in ER/EDR lib,
             * we use __MTK_BLE_ONLY_ENABLE__ to compartment it.
             */
#ifndef __MTK_BLE_ONLY_ENABLE__
                bt_gap_connection_handle_t phone_handle = bt_cm_get_gap_handle(phone_addr);
                bt_gap_exit_sniff_mode(phone_handle);
#endif
            }
        } else {
            ble_bap_disconnect_cis(p_msg->cis_handle);
        }

    }

#ifdef MTK_LE_CAP_STREAM_COMMON
    bt_sink_srv_cap_stream_check_cis_channel_info(p_msg->connect_handle);
#endif

    le_audio_log("[CAP][stream] check AM mode mapping, mode:%d, link_idx:%d", 2, mode, link_idx);
    if (valid_ase_num == p_msg->ase_id_list.num_of_ase && !g_cap_am_switch_suspending &&
	mode < CAP_AM_BROADCAST_MUSIC_MODE && (mode % CAP_UNICAST_DEVICE_NUM == link_idx) && bt_sink_srv_cap_am_is_psedev_streaming(mode)) {
        bt_sink_srv_cap_stream_set_cis_data_path_by_cis_handle(p_msg->connect_handle, p_msg->cis_handle, p_msg->ase_id_list);
    }
}

static void bt_sink_srv_cap_stream_ase_disable_stop_stream(bt_handle_t connect_handle, uint8_t ase_id, bt_sink_srv_cap_am_mode mode)
{
    bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
#if 1//MTK_COMMON:Todo: we should check and set the correct direction; otherwise, the controller may rsp w/ cmd disallowed.
    uint8_t path_direction[MAX_CIS_NUM] = {0x02, 0x02};
#else
    uint8_t path_direction[MAX_CIS_NUM] = {0x03, 0x03};
#endif
    uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(connect_handle, cis_list);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] disable_stop_stream, conn_hd:%d, ase id:%d", 2, connect_handle, ase_id);

    ble_bap_remove_cis_data_path(cis_num, cis_list, path_direction);
}

static void bt_sink_srv_cap_stream_ase_disabling_ind_handler(ble_bap_ase_disable_ind_t *p_msg)
{
    uint8_t direction = bt_sink_srv_cap_stream_get_ase_direction(p_msg->connect_handle, p_msg->ase_id);
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id);
    bt_sink_srv_cap_stream_send_ase_streaming_state_timer_stop(p_msg->connect_handle);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] disabling ind handler, conn_hd:%d, ase id:%d, state:%d, dir:%d", 4, p_msg->connect_handle, p_msg->ase_id, state, direction);

    if (!bt_sink_srv_cap_stream_is_disabling_state_ok(state)) {
        ble_bap_ase_disable_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_INVALID_ASE_STATE_MACHINE_TRANSITION, ERROR_REASON_NO_ERROR);

    } else if (bt_sink_srv_cap_stream_set_ase_sub_state(p_msg->connect_handle, p_msg->ase_id, SUB_STATE_DISABLING_RESPONDING)) {
        //bt_sink_srv_cap_stream_push_ase_id(p_msg->connect_handle, p_msg->ase_id);
        ble_bap_ase_id_list_t ase_id_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(p_msg->connect_handle, false);
        uint8_t link_index = bt_sink_srv_cap_get_link_index(p_msg->connect_handle);

        if (ase_id_list.num_of_ase == 1 &&
            AUDIO_DIRECTION_SOURCE == bt_sink_srv_cap_stream_get_ase_direction(p_msg->connect_handle, ase_id_list.ase_id[0])) {
            //for PTS testing, PTS will enable source ASE only and disable it.
            bt_sink_srv_cap_stream_ase_disable_stop_stream(p_msg->connect_handle, p_msg->ase_id, CAP_AM_UNICAST_MUSIC_MODE_START);
            if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START)) {
                ble_bap_ase_disable_response(p_msg->connect_handle, ase_id_list.ase_id[0], RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
            }
        } else if (ase_id_list.num_of_ase && p_msg->ase_id == ase_id_list.ase_id[ase_id_list.num_of_ase - 1]) {
            /*Check the last ASE direction*/
            if (AUDIO_DIRECTION_SINK == direction) {
                bt_sink_srv_cap_stream_ase_disable_stop_stream(p_msg->connect_handle, p_msg->ase_id, CAP_AM_UNICAST_MUSIC_MODE_START);
                if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START)) {
                    for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
                        ble_bap_ase_disable_response(p_msg->connect_handle, ase_id_list.ase_id[i], RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
                    }
                }
            } else if (AUDIO_DIRECTION_SOURCE == direction) {
                bt_sink_srv_cap_stream_ase_disable_stop_stream(p_msg->connect_handle, p_msg->ase_id, CAP_AM_UNICAST_CALL_MODE_START);
                if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_CALL_MODE_START)) {
                    for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
                        ble_bap_ase_disable_response(p_msg->connect_handle, ase_id_list.ase_id[i], RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
                    }
                }
            }
        }
    } else {
        /*Unable to complete disabling operation because ASE is proccessing other operation*/
        ble_bap_ase_disable_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_INSUFFICIENT_RESOURCES, ERROR_REASON_NO_ERROR);
    }
}

static void bt_sink_srv_cap_stream_cis_disconnected_notify_handler(ble_bap_cis_disconnected_notify_t *p_msg)
{
    bt_sink_srv_cap_stream_cis_disconnection_timer_stop();
    bt_sink_srv_cap_stream_send_ase_streaming_state_timer_stop(p_msg->connect_handle);

    if (g_cap_stream_service_info.max_link_num == bt_sink_srv_cap_get_link_index(p_msg->connect_handle)) {
        //MTK_COMMON: David - shrink log
        le_audio_log("[CAP][stream] cis disconnected notify handler, [ACL conn hd:%d] does not exist, [cis hd:%d]", 2, p_msg->connect_handle, p_msg->cis_handle);
        bt_sink_srv_cap_stream_ase_info_t *p_ase_info = bt_sink_srv_cap_stream_get_ase_link_by_cis_handle(p_msg->cis_handle);
        p_ase_info->cis_handle = BT_HANDLE_INVALID;
    } else {
        uint8_t state, sub_state = CAP_INVALID_UINT8, i = 0;
        bt_handle_t cis_handle;
        ble_bap_ase_id_list_t ase_list = bt_sink_srv_cap_stream_find_ase_id_list_with_cis_established(p_msg->connect_handle);

        for (i = 0; i < p_msg->ase_id_list.num_of_ase; i++) {
            state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
            sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
            cis_handle = bt_sink_srv_cap_stream_get_cis_handle(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);

            //MTK_COMMON: David - shrink log
            le_audio_log("[CAP][stream] cis disconnected notify handler, conn_hd:0x%x, ase id:%d, state:%d, cis hd:0x%x, disconnected cis hd:0x%x", 5,
                         p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], state, cis_handle, p_msg->cis_handle);

            bt_sink_srv_cap_stream_clear_cis_handle(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i]);
/*
 * MTK_COMMON: David
 * Since the CIS connection did not restricted within the status of ASEs,
 * we don't need to change sub state to QoS configuring when it already QoS configured.
 */
#if 1
            if (state == ASE_STATE_STREAMING || state == ASE_STATE_ENABLING) {
#else
            if (state == ASE_STATE_STREAMING || state == ASE_STATE_ENABLING || (state == ASE_STATE_QOS_CONFIGURED && sub_state == SUB_STATE_ENABLING_RESPONDING)) {
#endif /*end #if 1 //MTK_COMMON */
                if (!bt_sink_srv_cap_stream_set_ase_sub_state(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], SUB_STATE_QOS_CONFIG_RESPONDING) &&
                    sub_state != SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING) {
                    /*LE Audio AM task still proccessing, set next sub_state to stash next notification*/
                    /*If sub_state is SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING, this ASE will autonomously transit to qos configured state, don't need to stash next action*/
                    bt_sink_srv_cap_stream_set_ase_next_sub_state(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], SUB_STATE_QOS_CONFIG_RESPONDING);
                }
            } else if (state == ASE_STATE_IDLE) {
                configASSERT(0);
            } else if (state == ASE_STATE_RELEASING) {
                ble_bap_codec_config_response(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i],
                                              RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
            } else if (state != ASE_STATE_CODEC_CONFIGURED) {
                ble_bap_ase_qos_config_response(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i],
                                                RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
            }
        }

/*
 * MTK_COMMON: David
 * Since the CIS connection did not restricted within the status of ASEs,
 * we don't need to control AM when CIS disconnected, so AM stop will be controlled by ASE state directly.
 */
#if 0
        if (ase_list.num_of_ase) {
            uint8_t link_index = bt_sink_srv_cap_get_link_index(p_msg->connect_handle);

            if(bt_sink_srv_cap_stream_get_ase_direction(p_msg->connect_handle, ase_list.ase_id[ase_list.num_of_ase-1]) == AUDIO_DIRECTION_SINK)
            {
                if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START)) {
                    for (i = 0; i < p_msg->ase_id_list.num_of_ase; i++) {
                        ble_bap_ase_disable_response(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
                    }
                }
            }
            else if(bt_sink_srv_cap_stream_get_ase_direction(p_msg->connect_handle, ase_list.ase_id[ase_list.num_of_ase-1]) == AUDIO_DIRECTION_SOURCE)
            {
                if(1 == ase_list.num_of_ase)
                {
                    //PTS only enable source ASE
                    if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START)) {
                        ble_bap_ase_disable_response(p_msg->connect_handle, ase_list.ase_id[i], RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
                    }
                }
                else
                {
                    if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_CALL_MODE_START)) {
                        for (i = 0; i < p_msg->ase_id_list.num_of_ase; i++) {
                            ble_bap_ase_disable_response(p_msg->connect_handle, p_msg->ase_id_list.ase_id[i], RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
                        }
                    }
                }
            }

        }
#endif  /*end #if 0 //MTK_COMMON */
        //bt_sink_srv_cap_am_deinit(CAP_AM_DEINIT_REASON_CIS_DISCONNECT);
    }

    /*Check restart broadcast reception or not*/
    bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
    uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(p_msg->connect_handle, cis_list);
    bt_sink_srv_cap_stream_service_big_t *service_big = bt_sink_srv_cap_stream_get_service_big();
    le_audio_log("[CAP][stream] Check restart broadcast, cis_num:%d, bis_num:%d", 2,
        cis_num, g_cap_stream_service_info.big_info.num_bis);

    if (!cis_num && service_big != NULL && service_big->num_bis && bt_sink_srv_cap_am_get_current_mode() == CAP_AM_BROADCAST_MUSIC_MODE) {
        ble_bap_start_broadcast_reception(service_big->sync_handle, service_big->big_handle, service_big->num_bis, service_big->bis_indices);
    }
}

static void bt_sink_srv_cap_stream_ase_releasing_ind_handler(ble_bap_ase_release_ind_t *p_msg)
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id);
    uint8_t direction = bt_sink_srv_cap_stream_get_ase_direction(p_msg->connect_handle, p_msg->ase_id);
    bt_sink_srv_cap_stream_send_ase_streaming_state_timer_stop(p_msg->connect_handle);

    ble_bap_ase_response_reason_t response = {
        .response_code = RESPONSE_CODE_SUCCESS,
        .reason = ERROR_REASON_NO_ERROR
    };

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] releasing ind handler, conn_hd:0x%x, ase id:%d, state:%d", 3, p_msg->connect_handle, p_msg->ase_id, state);

    if (!bt_sink_srv_cap_stream_is_releasing_state_ok(state)) {
        response.response_code = RESPONSE_CODE_INVALID_ASE_STATE_MACHINE_TRANSITION;
        response.reason = ERROR_REASON_NO_ERROR;
    } else if (!bt_sink_srv_cap_stream_is_ase_link_set(p_msg->connect_handle, p_msg->ase_id)) {
        response.response_code = RESPONSE_CODE_INSUFFICIENT_RESOURCES;
        response.reason = ERROR_REASON_NO_ERROR;
    }

    if (state == ASE_STATE_STREAMING || state == ASE_STATE_ENABLING) { //Must stop Audio first, then send release notification
        //bt_sink_srv_cap_stream_set_ase_sub_state(p_msg->connect_handle, p_msg->ase_id, SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING);
        //bt_sink_srv_cap_stream_ase_disabling_ind_handler((ble_bap_ase_disable_ind_t*)p_msg);//Shall disable all ASEs first
        ble_bap_ase_id_list_t ase_id_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(p_msg->connect_handle, false);
        if (bt_sink_srv_cap_stream_set_ase_sub_state(p_msg->connect_handle, p_msg->ase_id, SUB_STATE_RELEASING_RESPONDING)) {

            uint8_t link_index = bt_sink_srv_cap_get_link_index(p_msg->connect_handle);

            if (ase_id_list.num_of_ase == 1 &&
                AUDIO_DIRECTION_SOURCE == bt_sink_srv_cap_stream_get_ase_direction(p_msg->connect_handle, ase_id_list.ase_id[0])) {
                //for PTS testing, PTS will enable source ASE only and disable it.
                bt_sink_srv_cap_stream_ase_disable_stop_stream(p_msg->connect_handle, p_msg->ase_id, CAP_AM_UNICAST_MUSIC_MODE_START);
                if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START)) {
                    ble_bap_ase_release_response(p_msg->connect_handle, ase_id_list.ase_id[0], response);
                }
            } else if (ase_id_list.num_of_ase && p_msg->ase_id == ase_id_list.ase_id[ase_id_list.num_of_ase - 1]) {
                /*Check the last ASE direction*/
                if (AUDIO_DIRECTION_SINK == direction) {
                    bt_sink_srv_cap_stream_ase_disable_stop_stream(p_msg->connect_handle, p_msg->ase_id, CAP_AM_UNICAST_MUSIC_MODE_START);
                    if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START)) {
                        for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
                            ble_bap_ase_release_response(p_msg->connect_handle, ase_id_list.ase_id[i], response);
                        }
                    }
                } else if (AUDIO_DIRECTION_SOURCE == direction) {
                    bt_sink_srv_cap_stream_ase_disable_stop_stream(p_msg->connect_handle, p_msg->ase_id, CAP_AM_UNICAST_CALL_MODE_START);
                    if (!bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_CALL_MODE_START)) {
                        for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
                            ble_bap_ase_release_response(p_msg->connect_handle, ase_id_list.ase_id[i], response);
                        }
                    }
                }
            }else {
                bt_sink_srv_cap_stream_set_ase_next_sub_state(p_msg->connect_handle, p_msg->ase_id, SUB_STATE_RELEASING_RESPONDING);
            }
        }
    } else {
        ble_bap_ase_release_response(p_msg->connect_handle, p_msg->ase_id, response);
    }
}

static void bt_sink_srv_cap_stream_ase_receiver_start_ready_ind_handler(ble_bap_ase_receiver_start_ready_ind_t *p_msg)
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id);
    bt_handle_t cis_handle = bt_sink_srv_cap_stream_get_cis_handle(p_msg->connect_handle, p_msg->ase_id);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] handshake ind handler, conn_hd:%d, state:%d, ase id:%d, cis handle:%d", 4,
                 p_msg->connect_handle, state, p_msg->ase_id, cis_handle);
    if (state != ASE_STATE_ENABLING) {
        configASSERT(0);
    }

    //bt_sink_srv_cap_stream_receiver_ready_handler(p_msg->connect_handle, p_msg->ase_id, true);
}

static void bt_sink_srv_cap_stream_ase_receiver_stop_ready_ind_handler(ble_bap_ase_receiver_start_ready_ind_t *p_msg)
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id);
    bt_handle_t cis_handle = bt_sink_srv_cap_stream_get_cis_handle(p_msg->connect_handle, p_msg->ase_id);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] Receiver Ready ind handler, conn_hd:%d, state:%d, ase id:%d, cis hd:%d", 4,
                 p_msg->connect_handle, state, p_msg->ase_id, cis_handle);
    if (state != ASE_STATE_DISABLING) {
        configASSERT(0);
    }

    //bt_sink_srv_cap_stream_receiver_ready_handler(p_msg->connect_handle, p_msg->ase_id, true);
}


static void bt_sink_srv_cap_stream_ase_update_metadata_ind_handler(ble_bap_ase_update_metadata_ind_t *p_msg)
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(p_msg->connect_handle, p_msg->ase_id);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] update metadata ind handler, conn_hd:%d, state:%d, ase id:%d", 3, p_msg->connect_handle, state, p_msg->ase_id);
    /*if(state != ASE_STATE_ENABLING && state != ASE_STATE_STREAMING)
    {
        ble_bap_ase_update_metadata_response(p_msg->connect_handle, p_msg->ase_id, BAP_RESPONSE_CODE_INVALID_ASE_STATE_MACHINE, ERROR_REASON_NO_ERROR);
    }
    else if(!bt_sink_srv_cap_stream_is_ase_link_set(p_msg->connect_handle, p_msg->ase_id))
    {
        ble_bap_ase_update_metadata_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_INSUFFICIENT_RESOURCES, ERROR_REASON_NO_ERROR);
    }
    else
    {
        ble_bap_ase_update_metadata_response(p_msg->connect_handle, p_msg->ase_id, RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
    }*/
}

static bool bt_sink_srv_cap_stream_is_enabling_state_ok(uint8_t state)
{
    //le_audio_log("[CAP][stream] is state ok to enable, state:%d", 1, state); //MTK_COMMON: David - reduce log
    if (state == ASE_STATE_ENABLING || state == ASE_STATE_QOS_CONFIGURED) {
        return true;
    }

    return false;
}

static bool bt_sink_srv_cap_stream_is_disabling_state_ok(uint8_t state)
{
    //MTK_COMMON: David - reduce log
    //le_audio_log("[CAP][stream] is state ok to disable, state:%d", 1, state);
    if (state == ASE_STATE_ENABLING || state == ASE_STATE_STREAMING) {
        return true;
    }

    return false;
}

static bool bt_sink_srv_cap_stream_is_releasing_state_ok(uint8_t state)
{
    le_audio_log("[CAP][stream] is state ok to release, state:%d", 1, state);
    if (state != ASE_STATE_IDLE && state != ASE_STATE_RELEASING) {
        return true;
    }

    return false;
}

static void bt_sink_srv_cap_stream_receiver_ready_handler(bt_handle_t connect_handle, uint8_t ase_id, bool is_start)
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(connect_handle, ase_id);
    uint8_t sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(connect_handle, ase_id);
    bt_handle_t cis_handle = bt_sink_srv_cap_stream_get_cis_handle(connect_handle, ase_id);

    le_audio_log("[CAP][stream] set data and send start, connect handle:0x%x, state:%d, sub state:%d, cis handle:0x%x, is start:%d", 5, connect_handle, state, sub_state, cis_handle, is_start);

    if (cis_handle != CAP_INVALID_UINT8
        && (state == ASE_STATE_ENABLING || state == ASE_STATE_DISABLING) && sub_state == CAP_INVALID_UINT8) {
        ble_bap_ase_receiver_start_stop_ready_response(connect_handle, ase_id,
                                                       RESPONSE_CODE_SUCCESS, ERROR_REASON_NO_ERROR);
    } else {
        ble_bap_ase_receiver_start_stop_ready_response(connect_handle, ase_id,
                                                       RESPONSE_CODE_INVALID_ASE_STATE_MACHINE_TRANSITION, ERROR_REASON_NO_ERROR);
    }
}

#ifndef MTK_LE_CAP_STREAM_COMMON
static uint8_t bt_sink_srv_cap_stream_pop_second_proccessing_ase_id(bt_handle_t connect_handle, bt_le_audio_direction_t direction, bool is_enabling)
#else
/* MTK_David: Change to public function for AM to check number of ASEs*/
uint8_t bt_sink_srv_cap_stream_pop_second_proccessing_ase_id(bt_handle_t connect_handle, bt_le_audio_direction_t direction, bool is_enabling)
#endif
{
    uint8_t link_index = 0, ase_index = 0;
    uint8_t count = 0;

    if ((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        if (is_enabling) {
            for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state == SUB_STATE_ENABLING_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].direction == direction) {
                    count++;
                    if (count == 2) {
                        return gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
                    }
                }
            }
        } else {
            for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state >= SUB_STATE_DISABLING_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state <= SUB_STATE_QOS_CONFIG_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].direction == direction) {
                    count++;
                    if (count == 2) {
                        return gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
                    }
                }
            }
        }
    }
    return CAP_INVALID_UINT8;
}

static uint8_t bt_sink_srv_cap_stream_pop_proccessing_ase_id(bt_handle_t connect_handle, bt_le_audio_direction_t direction, bool is_enabling)
{
    uint8_t link_index = 0, ase_index = 0;

    if ((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        if (is_enabling) {
            for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state == SUB_STATE_ENABLING_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].direction == direction) {
                    return gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
                }
            }
        } else {
            for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state >= SUB_STATE_DISABLING_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state <= SUB_STATE_QOS_CONFIG_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].direction == direction) {
                    return gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
                }
            }
        }
    }
    return CAP_INVALID_UINT8;

    ///
    /*bt_sink_srv_cap_stream_ase_queue_info_t *p_queue_info = bt_sink_srv_cap_stream_get_ase_queue(connect_handle);
    uint8_t value;

    if (p_queue_info == (bt_sink_srv_cap_stream_ase_queue_info_t *)NULL) {
        configASSERT(0);
        return CAP_INVALID_UINT8;
    } else if (p_queue_info->head == p_queue_info->tail) {
        configASSERT(0);
        return CAP_INVALID_UINT8;
    }

    value = p_queue_info->queue[p_queue_info->tail];

    le_audio_log("[CAP][stream] pop ase id, connect handle:%d, value:%d, tail:%d", 3, connect_handle, value, p_queue_info->tail);

    p_queue_info->tail = (p_queue_info->tail + 1) % CAP_MAX_ASE_NUM;

    return value;*/
}


static uint8_t bt_sink_srv_cap_stream_get_proccessing_ase_id_list(bt_handle_t connect_handle, bt_le_audio_direction_t direction, bool is_enabling, uint8_t *ase_id_list)
{
    uint8_t link_index = 0, ase_index = 0, count = 0;

    if (ase_id_list == NULL) {
        return 0;
    }

    if ((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        if (is_enabling) {
            for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state == SUB_STATE_ENABLING_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].direction == direction) {
                    ase_id_list[count] = gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
                    count++;
                }
            }

        } else {
            for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
                if (gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state >= SUB_STATE_DISABLING_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state <= SUB_STATE_QOS_CONFIG_RESPONDING &&
                    gp_cap_stream_link_info[link_index].ase_info[ase_index].direction == direction) {
                    ase_id_list[count] = gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
                    count++;
                }
            }
        }
    }

    return count;
}

static bool bt_sink_srv_cap_stream_cis_disconnection_timer_start(bt_handle_t cis_handle)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] cis disconn timer start, hd:0x%x", 1, cis_handle);

    bt_handle_t le_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
    if (BT_HANDLE_INVALID == le_handle) {
        return false;
    }

    uint32_t delay = 0;
    bt_gap_le_srv_conn_params_t *conn_params = (bt_gap_le_srv_conn_params_t *)bt_gap_le_srv_get_current_conn_params(le_handle);
    if (NULL == conn_params) {
        return false;
    } else if (conn_params->conn_interval) {
        delay = ((conn_params->conn_interval * 5) / 4) + (((conn_params->conn_interval * 5) % 4) ? 1 : 0);
    }

    le_audio_log("[CAP][stream] cis_disconnection delay:%d", 1, delay);

    if (bt_timer_ext_find(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID) == NULL) {
        bt_timer_ext_start(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID, (uint32_t)cis_handle, delay, bt_sink_srv_cap_stream_cis_disconnection_timer_callback);
    } else {
        bt_timer_ext_start(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID_2, (uint32_t)cis_handle, delay, bt_sink_srv_cap_stream_cis_disconnection_timer_callback);
    }
    //g_cap_stream_cis_disconnection_timer = xTimerCreate("cis_disconnection_timer", (delay * portTICK_PERIOD_MS), pdFALSE, NULL, bt_sink_srv_cap_stream_cis_disconnection_timer_callback);

    /*if (!g_cap_stream_cis_disconnection_timer) {
        return false;
    }

    xTimerStart(g_cap_stream_cis_disconnection_timer, 0);*/

    return true;
}

static bool bt_sink_srv_cap_stream_send_ase_streaming_state_timer_start(bt_handle_t le_handle, uint8_t ase_id)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] ase streaming timer start, le_hd:0x%x, ase_id:%d", 2, le_handle, ase_id);

    //bt_handle_t le_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
    if (BT_HANDLE_INVALID == le_handle) {
        return false;
    }

    uint32_t delay = 0;

    /*//Delay = 1 * connection interval
    bt_gap_le_srv_conn_params_t *conn_params = (bt_gap_le_srv_conn_params_t *)bt_gap_le_srv_get_current_conn_params(le_handle);
    if (NULL == conn_params) {
        return false;
    } else if (conn_params->conn_interval) {
        delay = ((conn_params->conn_interval * 5) / 4) + (((conn_params->conn_interval * 5) % 4) ? 1 : 0);
    }*/

    bt_sink_srv_cap_stream_ase_info_t *p_ase_info = bt_sink_srv_cap_stream_get_ase_link(le_handle, ase_id);
    if (p_ase_info != NULL) {
        //Delay = 6 * ISO interval
        delay = ((p_ase_info->iso_interval * 30) / 4) + (((p_ase_info->iso_interval * 30) % 4) ? 1 : 0);
    }

    le_audio_log("[CAP][stream] send_ase_streaming_state delay:%d", 1, delay);

    //bt_sink_srv_cap_stream_send_ase_streaming_state_data_t data = {0};
    //data.le_handle = le_handle;
    //.ase_id = ase_id;
    uint32_t data = (le_handle | (ase_id << 16));

    if(bt_timer_ext_find(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_1) == NULL) {
        g_ase_streaming_timer_data[0].le_handle = le_handle;
        g_ase_streaming_timer_data[0].ase_id = ase_id;
        bt_timer_ext_start(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_1, (uint32_t)data, delay, bt_sink_srv_cap_stream_send_ase_streaming_state_timer_callback);
    } else if(bt_timer_ext_find(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_2) == NULL){
        g_ase_streaming_timer_data[1].le_handle = le_handle;
        g_ase_streaming_timer_data[1].ase_id = ase_id;
        bt_timer_ext_start(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_2, (uint32_t)data, delay, bt_sink_srv_cap_stream_send_ase_streaming_state_timer_callback);
    } else if(bt_timer_ext_find(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_2) == NULL){
        g_ase_streaming_timer_data[2].le_handle = le_handle;
        g_ase_streaming_timer_data[2].ase_id = ase_id;
        bt_timer_ext_start(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_3, (uint32_t)data, delay, bt_sink_srv_cap_stream_send_ase_streaming_state_timer_callback);
    } else if(bt_timer_ext_find(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_2) == NULL){
        g_ase_streaming_timer_data[3].le_handle = le_handle;
        g_ase_streaming_timer_data[3].ase_id = ase_id;
        bt_timer_ext_start(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_4, (uint32_t)data, delay, bt_sink_srv_cap_stream_send_ase_streaming_state_timer_callback);
    }

    return true;
}

static bool bt_sink_srv_cap_stream_am_timer_stop(void)
{
    if (!g_cap_stream_am_timer) {
        return true;
    }

    if (pdPASS == xTimerDelete(g_cap_stream_am_timer, 0)) {
        g_cap_stream_am_timer = NULL;
        return true;
    }

    return false;
}

static bool bt_sink_srv_cap_stream_cis_disconnection_timer_stop(void)
{
    /*if (!g_cap_stream_cis_disconnection_timer) {
        return true;
    }

    if (pdPASS == xTimerDelete(g_cap_stream_cis_disconnection_timer, 0)) {
        g_cap_stream_cis_disconnection_timer = NULL;
        return true;
    }*/

#if 1 //MTK_COMMON: David - Check timer before stop it to reduce log
    if (bt_timer_ext_find(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID) &&
        BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID)) {
        return true;
    }

    if (bt_timer_ext_find(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID_2) &&
        BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID_2)) {
        return true;
    }
#else
    if (BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID)) {
        return true;
    }
    if (BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_CIS_DISCONNECT_TIMER_ID_2)) {
        return true;
    }
#endif

    return false;
}

static bool bt_sink_srv_cap_stream_send_ase_streaming_state_timer_stop(bt_handle_t stop_handle)
{
    bt_handle_t conn_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] ase streaming timer stop, hd:0x%x, conn_hd:0x%x", 2, stop_handle, conn_handle);

    if (stop_handle != conn_handle) {
        return false;
    }

#if 1 //MTK_COMMON: David - Check timer before stop it to reduce log
    if (bt_timer_ext_find(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_1) &&
        BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_1)) {
        g_ase_streaming_timer_data[0].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[0].ase_id = 0;
        return true;
    } else if (bt_timer_ext_find(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_2) &&
               BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_2)) {
        g_ase_streaming_timer_data[1].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[1].ase_id = 0;
        return true;
    } else if (bt_timer_ext_find(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_3) &&
               BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_3)) {
        g_ase_streaming_timer_data[2].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[2].ase_id = 0;
        return true;
    } else if (bt_timer_ext_find(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_4) &&
               BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_4)) {
        g_ase_streaming_timer_data[3].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[3].ase_id = 0;
        return true;
    }
#else
    if (BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_1)) {
        g_ase_streaming_timer_data[0].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[0].ase_id = 0;
        return true;
    } else if (BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_2)) {
        g_ase_streaming_timer_data[1].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[1].ase_id = 0;
        return true;
    } else if (BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_3)) {
        g_ase_streaming_timer_data[2].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[2].ase_id = 0;
        return true;
    } else if (BT_TIMER_EXT_STATUS_SUCCESS == bt_timer_ext_stop(BT_SINK_SRV_SEND_ASE_STREAMING_STATE_TIMER_ID_4)) {
        g_ase_streaming_timer_data[3].le_handle = BT_HANDLE_INVALID;
        g_ase_streaming_timer_data[3].ase_id = 0;
        return true;
    }
#endif

    return false;
}

static void bt_sink_srv_cap_stream_am_timer_callback(TimerHandle_t timer_id)
{
    if (pdPASS == xTimerDelete(g_cap_stream_am_timer, 0)) {
        g_cap_stream_am_timer = NULL;
    }

    bt_handle_t connect_handle = bt_sink_srv_cap_stream_get_service_ble_link();
    bt_sink_srv_cap_stream_release_autonomously(connect_handle, 0xFF, false, 0);
    //bt_sink_srv_cap_am_deinit();
}

static void bt_sink_srv_cap_stream_cis_disconnection_timer_callback(uint32_t timer_id, uint32_t data)
{
    ble_bap_ase_id_list_t ase_id_list = {0};
    bt_handle_t conn_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
    bt_handle_t cis_handle = (bt_handle_t)data;

    le_audio_log("[CAP][stream] cis_disconnection timer timeout: 0x%02x", 1, cis_handle);

    if (BT_HANDLE_INVALID != conn_handle) {
        ase_id_list = bt_sink_srv_cap_stream_find_ase_id_list_with_cis_established(conn_handle);
        if (ase_id_list.num_of_ase) {
            if (BT_HANDLE_INVALID != cis_handle && 0 != cis_handle) {
                if (ble_bap_disconnect_cis(cis_handle) == false) {
                    g_cap_stream_service_info.handle_to_disconnect = cis_handle;
                }
            }
        }
    }
}

static void bt_sink_srv_cap_stream_send_ase_streaming_state_timer_callback(uint32_t timer_id, uint32_t data)
{
    //bt_sink_srv_cap_stream_send_ase_streaming_state_data_t ase_info = (bt_sink_srv_cap_stream_send_ase_streaming_state_data_t)data;
    //bt_handle_t conn_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
    bt_handle_t le_handle = (bt_handle_t)data;
    uint8_t ase_id = (uint8_t)(data >> 16);

    le_audio_log("[CAP][stream] send_ase_streaming_state timer timeout, handle:0x%x, ase_id:%d", 2, le_handle, ase_id);

    if (BT_HANDLE_INVALID != le_handle) {

        for (uint8_t i = 0; i < CAP_MAX_CIS_NUM; i++) {
            if (g_ase_streaming_timer_data[i].le_handle == le_handle) {
                g_ase_streaming_timer_data[i].le_handle = BT_HANDLE_INVALID;
                g_ase_streaming_timer_data[i].ase_id = 0;
                break;
            }
        }

        if (AUDIO_DIRECTION_SINK == bt_sink_srv_cap_stream_get_ase_direction(le_handle, ase_id)) {
            bt_sink_srv_cap_stream_receiver_ready_handler(le_handle, ase_id, true);
        }
    }
}

/**************************************************************************************************
* Public Functions
**************************************************************************************************/
void bt_sink_srv_cap_stream_enabling_response(bt_handle_t handle, bool is_accept, bt_le_audio_direction_t direction)
{
    uint8_t ase_id = bt_sink_srv_cap_stream_pop_proccessing_ase_id(handle, direction, true);
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(handle, ase_id);
    uint8_t sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(handle, ase_id);
    uint8_t response_code;

    le_audio_log("[CAP][stream] enabling rsp, conn_hd:%d, ase id:%d, accept:%d, state:%d, sub state:%d", 5, handle, ase_id, is_accept, state, sub_state);

    if (!bt_sink_srv_cap_stream_is_ase_link_set(handle, ase_id)) {
        return;
    }

    if (is_accept == true && bt_sink_srv_cap_stream_is_enabling_state_ok(state)) {
        response_code = RESPONSE_CODE_SUCCESS;
        bt_sink_srv_cap_stream_set_ase_sub_state(handle, ase_id, SUB_STATE_ENABLING_NOTIFYING);
        /*if (BT_HANDLE_INVALID == bt_sink_srv_cap_stream_get_cis_handle(handle, ase_id)) {
            bt_sink_srv_cap_stream_am_timer_start();
        }*/
    } else {
        if (direction == AUDIO_DIRECTION_SOURCE) {
#if 1 //To inform APP that bidirectional LE audio streaming state, APP may need this event to apply different DSP algorithms.
            bt_sink_srv_bidirection_lea_state_update_t update = {0};
            update.le_handle = handle;
            update.state = BT_SINK_SRV_BIDIRECTION_LEA_STATE_DISABLE;
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_LE_BIDIRECTION_LEA_UPDATE, &update, sizeof(bt_sink_srv_bidirection_lea_state_update_t));
#endif
        #ifdef AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE
            bt_gap_le_srv_conn_info_t *conn_info = bt_gap_le_srv_get_conn_info(handle);
            if (conn_info != NULL) {
                bt_sink_srv_state_manager_notify_call_audio_state(BT_SINK_SRV_STATE_MANAGER_DEVICE_TYPE_LE, &conn_info->peer_addr.addr, BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED);
            }
        #endif
        }

        response_code = RESPONSE_CODE_INSUFFICIENT_RESOURCES;
        bt_sink_srv_cap_stream_set_ase_sub_state(handle, ase_id, CAP_INVALID_UINT8);
    }
    ble_bap_ase_enable_response(handle, ase_id, response_code, ERROR_REASON_NO_ERROR);
}

void bt_sink_srv_cap_stream_disabling_response(bt_handle_t handle, bool is_accept, bt_le_audio_direction_t direction)
{
    /*bt_sink_srv_cap_stream_ase_queue_info_t *p_queue_info = bt_sink_srv_cap_stream_get_ase_queue(handle);

    if(g_cap_stream_service_info.max_link_num == bt_sink_srv_cap_get_link_index(handle) ||
        (p_queue_info != NULL && p_queue_info->head == p_queue_info->tail))
    {
        //MTK_COMMON: David - shrink log
        le_audio_log("[CAP][stream] disabling rsp, no need to notify, [handle:%d] [ase_queue head:%d, tail:%d]", 3, handle, p_queue_info->head, p_queue_info->tail);
        return;
    }*/

    if (direction == AUDIO_DIRECTION_SOURCE) {
#if 1 //To inform APP that bidirectional LE audio streaming state, APP may need this event to apply different DSP algorithms.
        bt_sink_srv_bidirection_lea_state_update_t update = {0};
        update.le_handle = handle;
        update.state = BT_SINK_SRV_BIDIRECTION_LEA_STATE_DISABLE;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_LE_BIDIRECTION_LEA_UPDATE, &update, sizeof(bt_sink_srv_bidirection_lea_state_update_t));
#endif
    #ifdef AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE
        bt_gap_le_srv_conn_info_t *conn_info = bt_gap_le_srv_get_conn_info(handle);
        if (conn_info != NULL) {
            bt_sink_srv_state_manager_notify_call_audio_state(BT_SINK_SRV_STATE_MANAGER_DEVICE_TYPE_LE, &conn_info->peer_addr.addr, BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED);
        }
    #endif
    }

    uint8_t ase_id = bt_sink_srv_cap_stream_pop_proccessing_ase_id(handle, direction, false);
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(handle, ase_id);
    uint8_t sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(handle, ase_id);

    ble_bap_ase_response_reason_t response = {
        .response_code = RESPONSE_CODE_SUCCESS,
        .reason = ERROR_REASON_NO_ERROR
    };

    if (!bt_sink_srv_cap_stream_is_ase_link_set(handle, ase_id)) {
        return;
    }

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] disabling rsp, conn_hd:0x%x, ase id:%d, dir:%d, accept:%d, state:%d, sub state:%d", 6, handle, ase_id, direction, is_accept, state, sub_state);

    if ((!is_accept) || (!bt_sink_srv_cap_stream_is_disabling_state_ok(state))) {
        response.response_code = RESPONSE_CODE_INSUFFICIENT_RESOURCES;
    }

    if (sub_state == SUB_STATE_QOS_CONFIG_RESPONDING) {
        ble_bap_ase_qos_config_response(handle, ase_id, response.response_code, response.reason);
    } else if (sub_state == SUB_STATE_RELEASING_RESPONDING || sub_state == SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING) {
        ble_bap_ase_release_response(handle, ase_id, response);
    } else {
        ble_bap_ase_disable_response(handle, ase_id, response.response_code, response.reason);
    }
}

void bt_sink_srv_cap_stream_disabling_response_all(bt_handle_t handle, bool is_accept, bt_le_audio_direction_t direction)
{
    uint8_t ase_id_count, i, state, sub_state, ase_id;
    uint8_t ase_id_list[CAP_MAX_ASE_NUM] = {0};

    ble_bap_ase_response_reason_t response = {
        .response_code = RESPONSE_CODE_SUCCESS,
        .reason = ERROR_REASON_NO_ERROR
    };

    //MTK_COMMON: David - Check handle value first to avoid dummy log print
    if (handle == BT_HANDLE_INVALID)
        return;

    ase_id_count = bt_sink_srv_cap_stream_get_proccessing_ase_id_list(handle, direction, false, ase_id_list);

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] disabling rsp all, conn_hd:0x%x, dir:%d, ase count:%d, accept:%d", 4,
                 handle, direction, ase_id_count, is_accept);

    for (i = 0; i < ase_id_count; i++) {
        ase_id = ase_id_list[i];
        state = bt_sink_srv_cap_stream_get_ase_state(handle, ase_id);
        sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(handle, ase_id);

        //MTK_COMMON: David - reduce log
        le_audio_log("[CAP][stream] ase id[%d]:%d, state:%d(%d)", 4, i, ase_id, state, sub_state);

        if (!bt_sink_srv_cap_stream_is_ase_link_set(handle, ase_id)) {
            return;
        }

        if ((!is_accept) || (!bt_sink_srv_cap_stream_is_disabling_state_ok(state))) {
            response.response_code = RESPONSE_CODE_INSUFFICIENT_RESOURCES;
        }

        if (sub_state == SUB_STATE_QOS_CONFIG_RESPONDING) {
            ble_bap_ase_qos_config_response(handle, ase_id, response.response_code, response.reason);
        } else if (sub_state == SUB_STATE_RELEASING_RESPONDING || sub_state == SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING) {
            ble_bap_ase_release_response(handle, ase_id, response);
        } else {
            ble_bap_ase_disable_response(handle, ase_id, response.response_code, response.reason);
        }
    }
}

void bt_sink_srv_cap_stream_restarting_complete_response(bt_sink_srv_cap_am_mode mode)
{
    le_audio_log("[CAP][stream] restart complete rsp, mode:%d", 1, mode); //MTK_COMMON: David - shrink log
    if (mode <= CAP_AM_UNICAST_MUSIC_MODE_END) {
        bt_handle_t handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);
        bt_sink_srv_cap_stream_set_all_cis_data_path(handle);
        /*bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
        uint8_t path_id[MAX_CIS_NUM] = {1, 2};
        uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(handle, cis_list);
        ble_bap_set_cis_data_path(cis_num, &cis_list, &path_id);*/
    } else {
        bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();
        if (NULL != big_info) {
            ble_bap_set_bis_data_path(big_info->big_handle);
        }
    }
}

bt_status_t bt_sink_srv_cap_stream_set_big_sync_info(uint8_t big_handle, uint8_t num_bis, uint8_t *bis_indices)
{
    return ble_bap_config_sync_big(big_handle, num_bis, bis_indices);
}

bool bt_sink_srv_cap_stream_config_codec_autonomously(bt_handle_t connect_handle, bt_le_audio_direction_t direction, ble_ascs_config_codec_operation_t *parm)
{
    ble_bap_ase_codec_config_ind_t codec = {0};
    ble_ascs_config_codec_operation_ind_t ind = {0};
    ind.direction = direction;
    ind.param = parm;
    codec.connect_handle = connect_handle;
    codec.codec_config = &ind;

    if (bt_sink_srv_cap_stream_set_codec(&codec)) {
        if (ble_bap_codec_config_autonomously(connect_handle, (ble_bap_config_codec_operation_t *)parm)) {
            return true;
        }
    }
    return false;
}

bool bt_sink_srv_cap_stream_update_metadata_autonomously(bt_handle_t connect_handle, ble_ascs_update_metadata_operation_t *parm)
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(connect_handle, parm->ase_id);
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] Autonomous updatae metadata, conn_hd:%x, ase id:%d, state:%d", 3, connect_handle, parm->ase_id, state);

    if (ASE_STATE_STREAMING == state || ASE_STATE_ENABLING == state) {
        ble_bap_update_metadata_autonomously(connect_handle, parm);
        return true;
    }
    return false;
}

bool bt_sink_srv_cap_stream_disable_autonomously(bt_handle_t connect_handle, uint8_t ase_id)//Disable ASE in streaming or enabling state
{
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(connect_handle, ase_id);
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] Autonomous disable, conn_hd:%x, ase id:%d, state:%d", 3, connect_handle, ase_id, state);
    ble_bap_ase_id_list_t ase_id_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(connect_handle, false);
    bool is_ase_enabling = false;
    bt_sink_srv_cap_stream_send_ase_streaming_state_timer_stop(connect_handle);

    for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
        if (ase_id_list.ase_id[i] == ase_id) {
            is_ase_enabling = true;
            break;
        }
    }

    if (0xFF == ase_id && ase_id_list.num_of_ase) {
        is_ase_enabling = true;
    }

    if (!is_ase_enabling) {
        return false;
    }

    for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
        if (bt_sink_srv_cap_stream_is_ase_link_set(connect_handle, ase_id_list.ase_id[i]) && bt_sink_srv_cap_stream_set_ase_sub_state(connect_handle, ase_id_list.ase_id[i], SUB_STATE_DISABLING_RESPONDING)) {
            //bt_sink_srv_cap_stream_push_ase_id(connect_handle, ase_id_list.ase_id[i]);
            if ((i + 1) == ase_id_list.num_of_ase) {
                uint8_t link_index = bt_sink_srv_cap_get_link_index(connect_handle);
                if (bt_sink_srv_cap_stream_get_ase_direction(connect_handle, ase_id_list.ase_id[i]) == AUDIO_DIRECTION_SINK) {
                    bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START);
                } else if (bt_sink_srv_cap_stream_get_ase_direction(connect_handle, ase_id_list.ase_id[i]) == AUDIO_DIRECTION_SOURCE) {
                    bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_CALL_MODE_START);
                }
            }
        }
    }

    return true;
}

bool bt_sink_srv_cap_stream_hold_call(bt_handle_t connect_handle)
{
    bool status = true;
    /*Check call state*/
    ble_tbs_call_index_t incoming_call = bt_le_audio_sink_call_check_state(connect_handle, BLE_CCP_GTBS_INDEX, BLE_TBS_STATE_INCOMING);
    ble_tbs_call_index_t active_call = bt_le_audio_sink_call_check_state(connect_handle, BLE_CCP_GTBS_INDEX, BLE_TBS_STATE_ACTIVE);

    bt_le_audio_sink_call_action_param_t le_param;
    uint8_t buf[2] = {BLE_TBS_CALL_CONTROL_OPCODE_TYPE_LOCAL_HOLD, BLE_TBS_INVALID_CALL_INDEX};
    le_param.service_idx = BLE_CCP_GTBS_INDEX;
    le_param.length = 2;
    le_param.call_control_point = (ble_tbs_call_control_point_t *)buf;

    if (BLE_TBS_INVALID_CALL_INDEX != active_call) {
        buf[1] = active_call;
        bt_le_audio_sink_send_action(connect_handle, BT_LE_AUDIO_SINK_ACTION_CALL_HOLD, &le_param);
    } else if (BLE_TBS_INVALID_CALL_INDEX != incoming_call) {
        buf[1] = incoming_call;
        bt_le_audio_sink_send_action(connect_handle, BT_LE_AUDIO_SINK_ACTION_CALL_HOLD, &le_param);
    } else {
        status = false;
    }
    return status;
}

bool bt_sink_srv_cap_stream_retrieve_call(bt_handle_t connect_handle)
{
    bool status = true;
    /*Check call state*/
    ble_tbs_call_index_t local_held = BLE_TBS_INVALID_CALL_INDEX, remote_held = BLE_TBS_INVALID_CALL_INDEX, both_held = BLE_TBS_INVALID_CALL_INDEX;

    local_held = bt_le_audio_sink_call_check_state(connect_handle, BLE_CCP_GTBS_INDEX, BLE_TBS_STATE_LOCALLY_HELD);
    remote_held = bt_le_audio_sink_call_check_state(connect_handle, BLE_CCP_GTBS_INDEX, BLE_TBS_STATE_REMOTELY_HELD);
    both_held = bt_le_audio_sink_call_check_state(connect_handle, BLE_CCP_GTBS_INDEX, BLE_TBS_STATE_LOCALLY_AND_REMOTELY_HELD);

    bt_le_audio_sink_call_action_param_t le_param;
    uint8_t buf[2] = {BLE_TBS_CALL_CONTROL_OPCODE_TYPE_LOCAL_RETRIEVE, BLE_TBS_INVALID_CALL_INDEX};
    le_param.service_idx = BLE_CCP_GTBS_INDEX;
    le_param.length = 2;
    le_param.call_control_point = (ble_tbs_call_control_point_t *)buf;

    if (BLE_TBS_INVALID_CALL_INDEX != local_held) {
        buf[1] = local_held;
        bt_le_audio_sink_send_action(connect_handle, BT_LE_AUDIO_SINK_ACTION_CALL_RETRIEVE, &le_param);
    } else if (BLE_TBS_INVALID_CALL_INDEX != remote_held) {
        buf[1] = remote_held;
        bt_le_audio_sink_send_action(connect_handle, BT_LE_AUDIO_SINK_ACTION_CALL_RETRIEVE, &le_param);
    } else if (BLE_TBS_INVALID_CALL_INDEX != both_held) {
        buf[1] = both_held;
        bt_le_audio_sink_send_action(connect_handle, BT_LE_AUDIO_SINK_ACTION_CALL_RETRIEVE, &le_param);
    } else {
        status = false;
    }
    return status;
}

bool bt_sink_srv_cap_stream_release_autonomously(bt_handle_t connect_handle, uint8_t ase_id, bool resume_needed, uint16_t resume_timeout)
{
    bt_handle_t prev_handle = BT_HANDLE_INVALID, cis_handle = BT_HANDLE_INVALID;
    uint8_t direction, sub_state, data_path_direction = 0x03;
    uint8_t state = bt_sink_srv_cap_stream_get_ase_state(connect_handle, ase_id);
    ble_bap_ase_id_list_t ase_id_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(connect_handle, false);
    bool is_ase_enabling = false;
    bt_sink_srv_cap_stream_send_ase_streaming_state_timer_stop(connect_handle);

    ble_bap_ase_response_reason_t response = {
        .response_code = RESPONSE_CODE_SUCCESS,
        .reason = ERROR_REASON_NO_ERROR
    };

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] Autonomous releasing, conn_hd:0x%x, ase id:%d, state:%d, resume_needed:%d, resume_timeout:%d", 5,
                 connect_handle, ase_id, state, resume_needed, resume_timeout);

    for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
        if (ase_id_list.ase_id[i] == ase_id) {
            is_ase_enabling = true;
            break;
        }
    }

    if (0xFF == ase_id && ase_id_list.num_of_ase) {
        is_ase_enabling = true;
    } else if (0xFF == ase_id && !ase_id_list.num_of_ase) {
        /*return false;*/
    }

    if (is_ase_enabling) {
        for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
            cis_handle = bt_sink_srv_cap_stream_get_cis_handle(connect_handle, ase_id_list.ase_id[i]);

            if (BT_HANDLE_INVALID != cis_handle && prev_handle != cis_handle) {
                ble_bap_remove_cis_data_path(1, &cis_handle, &data_path_direction);
                prev_handle = cis_handle;
            }

            state = bt_sink_srv_cap_stream_get_ase_state(connect_handle, ase_id_list.ase_id[i]);
            sub_state = bt_sink_srv_cap_stream_get_ase_sub_state(connect_handle, ase_id_list.ase_id[i]);
            direction = bt_sink_srv_cap_stream_get_ase_direction(connect_handle, ase_id_list.ase_id[i]);

            le_audio_log("[CAP][stream] ase id:%d, state:%d, sub_state:%d, direction:%d", 4, ase_id_list.ase_id[i], state, sub_state, direction);

            if (bt_sink_srv_cap_stream_is_ase_link_set(connect_handle, ase_id_list.ase_id[i])/*&& bt_sink_srv_cap_stream_is_releasing_state_ok(state)*/) {
                /*if((ase_id == 0xFF) || (ase_id_list.ase_id[i] == ase_id))
                {
                    bt_sink_srv_cap_stream_set_ase_sub_state(connect_handle, ase_id_list.ase_id[i],SUB_STATE_RELEASING_RESPONDING);
                }
                else
                {
                    bt_sink_srv_cap_stream_set_ase_sub_state(connect_handle, ase_id_list.ase_id[i],SUB_STATE_DISABLING_RESPONDING);
                }*/

                if (state == ASE_STATE_STREAMING || state == ASE_STATE_ENABLING || (state == ASE_STATE_QOS_CONFIGURED && (sub_state == SUB_STATE_ENABLING_RESPONDING || sub_state == SUB_STATE_ENABLING_NOTIFYING))) { //Audio stop first, then send notification
                    if (bt_sink_srv_cap_stream_set_ase_sub_state(connect_handle, ase_id_list.ase_id[i], SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING)) {
                        //bt_sink_srv_cap_stream_push_ase_id(connect_handle, ase_id_list.ase_id[i]);
                        if ((i + 1) == ase_id_list.num_of_ase) {
                            uint8_t link_index = bt_sink_srv_cap_get_link_index(connect_handle);

                            if (AUDIO_DIRECTION_SINK == direction) {
                                bt_sink_srv_cap_event_media_change_state_t media_state;
                                media_state.connect_handle = connect_handle;
                                media_state.resume = resume_needed;
                                le_audio_sink_inform_app(BT_LE_AUDIO_SINK_EVENT_MEDIA_SUSPEND, &media_state);
                                if (resume_needed) {
                                    bt_sink_srv_cap_am_audio_suspend(link_index + CAP_AM_UNICAST_MUSIC_MODE_START);
                                } else {
                                    bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START);
                                }
#if (PAUSE_LE_MUSIC_WHEN_INTERRUPT)
                                bt_le_audio_sink_action_param_t le_param = {
                                    .service_idx = 0xFF,
                                };

                                bt_le_audio_sink_send_action(connect_handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PAUSE, &le_param);
#endif
                            } else if(direction == AUDIO_DIRECTION_SOURCE) {
                                if (true == bt_sink_srv_cap_stream_is_source_ase_only(connect_handle)) {
                                    bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_MUSIC_MODE_START);
                                } else {
                                    bt_sink_srv_cap_am_audio_stop(link_index + CAP_AM_UNICAST_CALL_MODE_START);
                                }
                            }
                        }
                    } else {
                        bt_sink_srv_cap_stream_set_ase_next_sub_state(connect_handle, ase_id_list.ase_id[i], SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING);
                    }
                }
            }
        }
    } else {
        ase_id_list = bt_sink_srv_cap_stream_find_ase_id_list_with_cis_established(connect_handle);

        if (ase_id == 0xFF) {
            if (!ase_id_list.num_of_ase) {
                return false;
            }

            for (uint8_t i = 0; i < ase_id_list.num_of_ase; i++) {
                cis_handle = bt_sink_srv_cap_stream_get_cis_handle(connect_handle, ase_id_list.ase_id[i]);

                if (BT_HANDLE_INVALID != cis_handle && prev_handle != cis_handle) {
                    ble_bap_remove_cis_data_path(1, &cis_handle, &data_path_direction);
                    prev_handle = cis_handle;
                }

                state = bt_sink_srv_cap_stream_get_ase_state(connect_handle, ase_id_list.ase_id[i]);

                if (bt_sink_srv_cap_stream_is_ase_link_set(connect_handle, ase_id_list.ase_id[i]) && bt_sink_srv_cap_stream_is_releasing_state_ok(state)) {
                    bt_sink_srv_cap_stream_set_ase_sub_state(connect_handle, ase_id_list.ase_id[i], SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING);
                    ble_bap_ase_release_response(connect_handle, ase_id_list.ase_id[i], response);
                }
            }
        } else {
            state = bt_sink_srv_cap_stream_get_ase_state(connect_handle, ase_id);

            if (bt_sink_srv_cap_stream_is_ase_link_set(connect_handle, ase_id) && bt_sink_srv_cap_stream_is_releasing_state_ok(state)) {
                bt_sink_srv_cap_stream_set_ase_sub_state(connect_handle, ase_id, SUB_STATE_AUTONOMOUSLY_RELEASING_RESPONDING);
                ble_bap_ase_release_response(connect_handle, ase_id, response);
            } else {
                return false;
            }
        }
    }

    return true;
}

bt_handle_t bt_sink_srv_cap_stream_get_service_ble_link(void)
{
    return g_cap_stream_service_info.service_connect_handle;
}

bt_handle_t bt_sink_srv_cap_stream_get_ble_link_with_cis_established(void)
{
    return g_cap_stream_service_info.le_handle_with_cis_established;
}

bt_handle_t bt_sink_srv_cap_get_ble_link_by_streaming_mode(bt_sink_srv_cap_am_mode mode)
{
    if (mode > CAP_AM_UNICAST_MUSIC_MODE_END) {
        return BT_HANDLE_INVALID;
    }

    return bt_sink_srv_cap_get_link_handle(mode % CAP_UNICAST_DEVICE_NUM);
}

void bt_sink_srv_cap_stream_allocate_ase_link(bt_handle_t connect_handle)
{
    for (uint8_t ase_index = 1; ase_index <= CAP_MAX_ASE_NUM; ase_index++) {
        bt_sink_srv_cap_stream_set_ase_link(connect_handle, ase_index);
    }
    /*bt_sink_srv_cap_stream_set_ase_link(connect_handle, DEFAULT_ASE_ID_1);
    bt_sink_srv_cap_stream_set_ase_link(connect_handle, DEFAULT_ASE_ID_2);
    bt_sink_srv_cap_stream_set_ase_link(connect_handle, DEFAULT_ASE_ID_3);*/
}

void bt_sink_srv_cap_stream_clear_all_ase_link(bt_handle_t connect_handle)
{
    uint8_t link_index = 0, ase_index;

    //MTK_COMMON: David - reduce log
    //le_audio_log("[CAP][stream] clear all ase link , connection handle: %x", 1, connect_handle);
    if ((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num) {
        for (ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++) {
            gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id = CAP_INVALID_UINT8;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].cis_handle = BT_HANDLE_INVALID;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_state = ASE_STATE_IDLE;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state = CAP_INVALID_UINT8;
            gp_cap_stream_link_info[link_index].ase_info[ase_index].iso_interval = 0;
            if (gp_cap_stream_link_info[link_index].ase_info[ase_index].codec_specific_configuration) {
                vPortFree(gp_cap_stream_link_info[link_index].ase_info[ase_index].codec_specific_configuration);
                gp_cap_stream_link_info[link_index].ase_info[ase_index].codec_specific_configuration = NULL;
            }
            if (gp_cap_stream_link_info[link_index].ase_info[ase_index].metadata) {
                vPortFree(gp_cap_stream_link_info[link_index].ase_info[ase_index].metadata);
                gp_cap_stream_link_info[link_index].ase_info[ase_index].metadata = NULL;
            }

        }
    }
    bt_sink_srv_cap_stream_clear_service_ble_link(connect_handle);
    bt_sink_srv_cap_stream_cis_disconnection_timer_stop();
    bt_sink_srv_cap_stream_send_ase_streaming_state_timer_stop(connect_handle);
}

bool bt_sink_srv_cap_stream_clear_service_ble_link(bt_handle_t connect_handle)
{
    //MTK_COMMON: David - Check handle value first to avoid dummy log print
    if (connect_handle == BT_HANDLE_INVALID)
        return false;

    if (bt_sink_srv_cap_stream_is_ble_link_serviced(connect_handle)) {
        //MTK_COMMON: David - reduce log
        //le_audio_log("[CAP][stream] clear service ble link, SUCCESS, conn_hd:%d", 1, connect_handle);
        g_cap_stream_service_info.service_connect_handle = BT_HANDLE_INVALID;
        return true;
    }

    le_audio_log("[CAP][stream] clear service ble link, FAIL, conn_hd:%d", 1, connect_handle); //MTK_COMMON: David - shrink log
    return false;
}

bool bt_sink_srv_cap_stream_am_timer_start(void)
{
    le_audio_log("[CAP][stream] am timer start, 0x%X", 1, g_cap_stream_am_timer); //MTK_COMMON: David - shrink log
    if (g_cap_stream_am_timer) {
        return false;
    } else {
        g_cap_stream_am_timer = xTimerCreate("am_init_timer", (10000 * portTICK_PERIOD_MS), pdFALSE, NULL, bt_sink_srv_cap_stream_am_timer_callback);
    }

    if (!g_cap_stream_am_timer) {
        return false;
    }

    xTimerStart(g_cap_stream_am_timer, 0);

    return true;
}
#endif
ble_bap_ase_id_list_t bt_sink_srv_cap_stream_find_streaming_ase_id_list(bt_handle_t connect_handle, bool is_processing)
{
    uint8_t link_index = 0, ase_index;
    ble_bap_ase_id_list_t ase_id_list = {0};
    bt_sink_srv_cap_stream_ase_info_t *ase_info = NULL;

    if((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num)
    {
        for(ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++)
        {
            ase_info = &gp_cap_stream_link_info[link_index].ase_info[ase_index];
            if(bt_sink_srv_cap_stream_check_ase_streaming_and_processing(ase_info, is_processing) && ase_id_list.num_of_ase <= MAX_STREAMING_ASE_NUM)
            {
                ase_id_list.ase_id[ase_id_list.num_of_ase] = ase_info->ase_id;
                ase_id_list.num_of_ase++;
            }
        }
    }
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] find streaming ase list, [num:%d], [ase_id: %x %x %x]", 4,
                 ase_id_list.num_of_ase, ase_id_list.ase_id[0], ase_id_list.ase_id[1], ase_id_list.ase_id[2]);
    return ase_id_list;
}

bt_le_audio_direction_t bt_sink_srv_cap_stream_find_streaming_ase_direction(bt_handle_t connect_handle, bool is_processing)
{
    uint8_t link_index = 0, ase_index;
    ble_bap_ase_id_list_t ase_id_list = {0};
    bt_sink_srv_cap_stream_ase_info_t *ase_info = NULL;

    if((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num)
    {
        for(ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++)
        {
            ase_info = &gp_cap_stream_link_info[link_index].ase_info[ase_index];
            if(bt_sink_srv_cap_stream_check_ase_streaming_and_processing(ase_info, is_processing) && ase_id_list.num_of_ase <= MAX_STREAMING_ASE_NUM)
            {
                ase_id_list.ase_id[ase_id_list.num_of_ase] = ase_info->ase_id;
                ase_id_list.num_of_ase++;
                if (ase_info->direction == AUDIO_DIRECTION_SOURCE) {
                    le_audio_log("[CAP][stream] bt_sink_srv_cap_stream_find_streaming_ase_direction[%d], SOURCE [ase_id:%d] ", 2,
                        is_processing, ase_info->ase_id);
                    return AUDIO_DIRECTION_SOURCE;
                }
            }
        }
    }

    if (ase_id_list.num_of_ase) {
        //MTK_COMMON: David - shrink log
        le_audio_log("[CAP][stream] find ase dir[%d], SINK [ase_id:%d]", 2,
                     is_processing, ase_id_list.ase_id[ase_id_list.num_of_ase-1]);
        return AUDIO_DIRECTION_SINK;
    }

    le_audio_log("[CAP][stream] find precess and ase dir, FAIL hd:%x", 1, connect_handle); //MTK_COMMON: David - shrink log
    return CAP_INVALID_UINT8;
}

bool bt_sink_srv_cap_stream_init(uint8_t max_link_num)
{
    if (BT_STATUS_SUCCESS != ble_bap_init(bt_sink_srv_cap_stream_callback, max_link_num)) {
        le_audio_log("[CAP][stream] init, FAIL, bap fail", 0);
        return false;
    }
#ifdef AIR_LE_AUDIO_CIS_ENABLE
    //MTK_Titan:change coding method for easy reading
    uint32_t info_size = sizeof(bt_sink_srv_cap_stream_link_info_t) * max_link_num;
    if ((gp_cap_stream_link_info = (bt_sink_srv_cap_stream_link_info_t *)pvPortMalloc(info_size)) == NULL) {
        le_audio_log("[CAP][stream] init, FAIL, malloc fail", 0);
        return false;
    }
    //MTK_Titan:make sure the struct is clean due to bt_sink_srv_cap_stream_parameter_init may not init all.
    memset(gp_cap_stream_link_info, 0x0, info_size);
#endif

    bt_sink_srv_cap_stream_parameter_init(max_link_num);

    le_audio_log("[CAP][stream] init, SUCCESS, max link:%d", 1, max_link_num);

    return true;
}

bool bt_sink_srv_cap_stream_deinit(void)
{
    if (BT_STATUS_SUCCESS != ble_bap_deinit(bt_sink_srv_cap_stream_callback)) {
        le_audio_log("[CAP][stream] deinit, FAIL", 0);
        return false;
    }

    le_audio_log("[CAP][stream] deinit, SUCCESS", 0);
    return true;
}

static void bt_sink_srv_cap_stream_big_sync_established_notify_handler(ble_bap_big_sync_established_notify_t *p_msg)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] big establish notify, status:0x%X", 1, ((ble_bap_big_sync_established_notify_t *)p_msg)->status);
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_current_mode();
    bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();

    if (NULL == big_info) {
        le_audio_log("[CAP][stream] Null BIG info", 0);
        return;
    }

    if (CAP_AM_BROADCAST_MUSIC_MODE == mode) {
        if (!g_cap_am_switch_suspending) {
            ble_bap_set_bis_data_path(big_info->big_handle);
        }
        if (!g_broadcast_keep_pa) {
            ble_bap_stop_syncing_to_periodic_advertising(big_info->sync_handle);
        }
        bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BIG_SYNC_ESTABLISHED, p_msg);
    } else {
        ble_bap_stop_broadcast_reception(big_info->big_handle);
    }
}

static void bt_sink_srv_cap_stream_set_service_big(bt_handle_t sync_handle, uint8_t big_handle, uint8_t num_bis, uint8_t *bis_indices)
{
    le_audio_log("[CAP][stream] set service bis, sync_handle:%04x, num_bis:%d", 2, sync_handle, num_bis);

    g_cap_stream_service_info.big_info.sync_handle = sync_handle;
    g_cap_stream_service_info.big_info.big_handle = big_handle;
    g_cap_stream_service_info.big_info.num_bis = num_bis;

    if (g_cap_stream_service_info.big_info.bis_indices != NULL) {
        vPortFree(g_cap_stream_service_info.big_info.bis_indices);
    }

    if (num_bis) {
        g_cap_stream_service_info.big_info.bis_indices = pvPortMalloc(num_bis);
        memcpy(g_cap_stream_service_info.big_info.bis_indices, bis_indices, num_bis);
    }
}

bool bt_sink_srv_cap_stream_clear_service_big(uint8_t big_handle)
{
    bt_sink_srv_cap_stream_service_big_t *service_big = bt_sink_srv_cap_stream_get_service_big();

    if (big_handle == service_big->big_handle) {
        le_audio_log("[CAP][stream] clear service big, SUCCESS, big handle:%d", 1, big_handle);
        service_big->sync_handle = BT_HANDLE_INVALID;
        service_big->big_handle = 0;
        service_big->num_bis = 0;
        if (service_big->bis_indices != NULL) {
            vPortFree(g_cap_stream_service_info.big_info.bis_indices);
            g_cap_stream_service_info.big_info.bis_indices = NULL;
        }

        if (service_big->config_info.metadata != NULL) {
            vPortFree(service_big->config_info.metadata);
            service_big->config_info.metadata = NULL;
        }
        memset(&service_big->config_info.codec[0], 0, sizeof(bt_sink_srv_cap_stream_config_info_t));
        return true;
    }

    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] clear service big, FAIL, conn_hd:%d", 1, big_handle);
    return false;
}

bt_sink_srv_cap_stream_service_big_t *bt_sink_srv_cap_stream_get_service_big(void)
{
    return &g_cap_stream_service_info.big_info;
}

bool bt_sink_srv_cap_stream_is_broadcast_streaming(void)
{
    bt_sink_srv_cap_stream_service_big_t *service_big = bt_sink_srv_cap_stream_get_service_big();
    if ((NULL != service_big) && (service_big->big_handle)) {
        return true;
    }
    return false;
}

void bt_sink_srv_cap_stream_get_default_bmr_scan_info(bt_sink_srv_cap_stream_bmr_scan_param_t *scan_param)
{
    scan_param->audio_channel_allocation = g_default_bmr_scan_info.audio_channel_allocation;
    scan_param->bms_address = (bt_bd_addr_t *)&g_default_bmr_scan_info.bms_address;
}

bt_sink_srv_cap_stream_config_info_t bt_sink_srv_cap_stream_get_config_info(bt_sink_srv_cap_am_mode mode, bt_le_audio_direction_t direction)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] get config info, mode:%d, dir:%d", 2, mode, direction);
    bt_sink_srv_cap_stream_config_info_t config_info = {{0}};
    uint8_t *is_stereo;


    if (mode == CAP_AM_BROADCAST_MUSIC_MODE && direction == AUDIO_DIRECTION_SINK) {
        bt_sink_srv_cap_stream_service_big_t *service_big = bt_sink_srv_cap_stream_get_service_big();

        if((NULL != service_big) && service_big->sync_handle && service_big->sync_handle != BT_HANDLE_INVALID) {
            if (!ble_bap_get_broadcast_config_info(service_big->sync_handle, (ble_bap_stream_config_info_t*)&config_info)) {
                /*PA is not synced, get default config info*/
                config_info = service_big->config_info;
            }
        } else {
            le_audio_log("get config info FAIL!", 0); //MTK_COMMON: David - shrink log
        }

    }
#ifdef AIR_LE_AUDIO_CIS_ENABLE
    else {
        bt_sink_srv_cap_stream_ase_info_t *p_info = NULL;

        bt_handle_t handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);
        uint8_t ase_id = bt_sink_srv_cap_stream_pop_proccessing_ase_id(handle, direction, true);
        ble_bap_ase_id_list_t ase_list = {0};
        if (CAP_INVALID_UINT8 == ase_id) {
            /*Restarting streaming, ASE's sub state is not enabling responding */
            ase_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(handle, false);
            for (uint8_t i = 0; i < ase_list.num_of_ase; i++) {
                if (direction == bt_sink_srv_cap_stream_get_ase_direction(handle, ase_list.ase_id[i])) {
                    ase_id = ase_list.ase_id[i];
                    break;
                }
            }
        }

        if ((p_info = bt_sink_srv_cap_stream_get_ase_link(handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t *)NULL) {
            memcpy(&config_info.codec[0], &p_info->codec[0], AUDIO_CODEC_ID_SIZE);
            config_info.sampling_frequency = (uint8_t)(*(ble_ascs_get_ltv_value_from_codec_specific_configuration(CODEC_CONFIGURATION_TYPE_SAMPLING_FREQUENCY,
                                                                                                                  p_info->codec_specific_configuration_length, p_info->codec_specific_configuration)));
            memcpy(&config_info.sdu_interval[0], &p_info->sdu_interval[0], SDU_INTERVAL_SIZE);

            is_stereo = ble_ascs_get_ltv_value_from_codec_specific_configuration(CODEC_CONFIGURATION_TYPE_AUDIO_CHANNEL_ALLOCATION,
                                                                                 p_info->codec_specific_configuration_length, p_info->codec_specific_configuration);

            config_info.is_stereo = false;

            if (is_stereo != NULL && *is_stereo == 3) {
                config_info.is_stereo = true;
            }

            //MTK_COMMON: David - reduce log
            //le_audio_log("config_info.sampling_frequency:%x, frame_length:%d, is_stereo:%d", 3, config_info.sampling_frequency,
            //    p_info->maximum_sdu_size, config_info.is_stereo);
            config_info.frame_payload_length = p_info->maximum_sdu_size;
            config_info.metadata_length = p_info->metadata_length;
            config_info.metadata = p_info->metadata;
#ifdef MTK_LE_CAP_STREAM_COMMON
            memcpy(config_info.presentation_delay, p_info->presentation_delay, PRESENTATION_DELAY_SIZE);
#endif
            //MTK_COMMON: David - reduce log
            //le_audio_log("config_info.metadata_length:%d", 1, config_info.metadata_length);
        } else {
            le_audio_log("get config info FAIL!", 0); //MTK_COMMON: David - shrink log
        }
    }
#endif
///
    /*if(streaming_mode == UNICAST_MUSIC_MODE || streaming_mode == UNICAST_CALL_MODE) {
        bt_sink_srv_cap_stream_ase_info_t *data = NULL;
        bt_handle_t connect_handle = bt_sink_srv_cap_stream_get_service_ble_link();
        uint8_t ase_id = bt_sink_srv_cap_stream_pop_proccessing_ase_id(connect_handle);
        bt_sink_srv_cap_stream_push_ase_id(connect_handle, ase_id);

        if((data = bt_sink_srv_cap_stream_get_ase_link(connect_handle, ase_id)) != (bt_sink_srv_cap_stream_ase_info_t*)NULL)
        {
            //le_audio_log("bt_sink_srv_cap_stream_get_config_info SUCCESS!", 0);
            memcpy(&config_info.codec[0], &data->codec[0], AUDIO_CODEC_ID_SIZE);
            //config_info.direction = data->direction;
            //config_info.codec_specific_configuration_length = data->codec_specific_configuration_length;
            //config_info.codec_specific_configuration = data->codec_specific_configuration;
            config_info.sampling_frequency = (uint8_t)(*(ble_ascs_get_ltv_value_from_codec_specific_configuration(CODEC_CONFIGURATION_TYPE_SAMPLING_FREQUENCY,
                data->codec_specific_configuration_length, data->codec_specific_configuration)));
            le_audio_log(" config_info.sampling_frequency:%x", 1, config_info.sampling_frequency);
            memcpy(&config_info.sdu_interval[0], &data->sdu_interval[0], SDU_INTERVAL_SIZE);
            //config_info.framing = data->framing;
            //config_info.phy = data->phy;
            //memcpy(&config_info.presentation_delay[0], &data->presentation_delay[0], PRESENTATION_DELAY_SIZE);
            //config_info.retransmission_number = data->retransmission_number;
            config_info.frame_payload_length = data->maximum_sdu_size;
            //config_info.transport_latency = data->transport_latency;
        }
        else
        {
            le_audio_log("bt_sink_srv_cap_stream_get_config_info FAIL!", 0);
        }
    } else if(streaming_mode == BROADCAST_MUSIC_MODE) {
        bt_sink_srv_cap_stream_service_big_t *service_big = bt_sink_srv_cap_stream_get_service_big();

        if((NULL != service_big) && service_big->sync_handle && service_big->sync_handle != BT_HANDLE_INVALID) {
            ble_bap_get_broadcast_config_info(service_big->sync_handle, (ble_bap_stream_config_info_t*)&config_info);
        } else {
            le_audio_log("bt_sink_srv_cap_stream_get_config_info FAIL!", 0);
        }
    }*/

    return config_info;
}

uint8_t bt_sink_srv_cap_stream_get_bis_index_by_audio_location(bt_le_audio_location_t audio_location, uint8_t subgroup_idx)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] get bis index by audio location, location:%04X, subgroup_idx:%d", 2, audio_location, subgroup_idx);
    uint8_t *temp = NULL;
    uint32_t alloction = AUDIO_LOCATION_NONE;
    ble_bap_basic_audio_announcements_level_1_t *level_1 = ble_bap_get_basic_audio_announcements_level_1(g_default_bmr_scan_info.sync_handle);
    if (NULL != level_1 && subgroup_idx < level_1->num_subgroups) {
        for (uint8_t i = 0; i < level_1->level_2[subgroup_idx].num_bis; i++) {
            temp = ble_ascs_get_ltv_value_from_codec_specific_configuration(CODEC_CONFIGURATION_TYPE_AUDIO_CHANNEL_ALLOCATION,
                                                                            level_1->level_2[subgroup_idx].level_3[i].codec_specific_configuration_length, level_1->level_2[subgroup_idx].level_3[i].codec_specific_configuration);
            if (NULL != temp) {
                alloction = (temp[0] | (temp[1] << 8) | (temp[2] << 16) | (temp[3] << 24));
                if ((alloction & audio_location) == audio_location) {
                    le_audio_log("[CAP][stream] Found bis_index:%d", 1, level_1->level_2[subgroup_idx].level_3[i].bis_index);
                    return level_1->level_2[subgroup_idx].level_3[i].bis_index;
                }
            }
        }

    }
    return BIS_INDEX_INVALID;
}

uint8_t bt_sink_srv_cap_stream_get_bis_index_in_pa_level_1(bt_le_audio_location_t audio_location)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] get bis index, location:%04X", 1, audio_location);
    uint8_t bis_index = BIS_INDEX_INVALID;
    ble_bap_basic_audio_announcements_level_1_t *level_1 = ble_bap_get_basic_audio_announcements_level_1(g_default_bmr_scan_info.sync_handle);
    if (NULL != level_1) {
        for (uint8_t i = 0; i < level_1->num_subgroups; i++) {
            bis_index = bt_sink_srv_cap_stream_get_bis_index_by_audio_location(audio_location, i);
            if (BIS_INDEX_INVALID != bis_index) {
                return bis_index;
            }
        }
    }
    return BIS_INDEX_INVALID;
}

bt_status_t bt_sink_srv_cap_stream_scan_broadcast_source(bt_sink_srv_cap_stream_bmr_scan_param_t *scan_param)
{
    bt_sink_srv_state_t sink_srv_state = bt_sink_srv_get_state();
    bt_handle_t le_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
    le_audio_log("[CAP][stream] scan broadcast source, sink state:%x", 1, sink_srv_state); //MTK_COMMON: David - shrink log

    if (sink_srv_state >= BT_SINK_SRV_STATE_INCOMING || bt_sink_srv_cap_am_get_current_mode() <= CAP_AM_UNICAST_CALL_MODE_END) {
        /*Call is doing, shall not be interrupted*/
        return BT_STATUS_FAIL;
    } else if (sink_srv_state == BT_SINK_SRV_STATE_STREAMING) {
        /*A2DP streaming, shall pause music first*/
        //bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PAUSE, NULL);
    }

    if (le_handle != BT_HANDLE_INVALID) {
        //bt_sink_srv_cap_stream_release_autonomously(le_handle, 0xFF, false, 0);
    }

    g_default_bmr_scan_info.audio_channel_allocation = scan_param->audio_channel_allocation;
    g_default_bmr_scan_info.sync_policy = scan_param->sync_policy;

    if (NULL == scan_param->bms_address) {
        memset(&g_default_bmr_scan_info.bms_address[0], 0, sizeof(bt_bd_addr_t));
        g_default_bmr_scan_info.specified_bms = false;
    } else {
        memcpy(&g_default_bmr_scan_info.bms_address[0], &scan_param->bms_address[0], sizeof(bt_bd_addr_t));
        g_default_bmr_scan_info.specified_bms = true;
    }

    return ble_bap_scan_broadcast_source(scan_param->duration, scan_param->scan_type);

    /*if (BT_STATUS_SUCCESS == ble_bap_scan_broadcast_source()) {
        if (NULL != scan_param->bms_address) {
            memcpy(&g_default_bmr_scan_info.bms_address[0], &scan_param->bms_address[0], sizeof(bt_bd_addr_t));
            g_default_bmr_scan_info.specified_bms = true;
        }
    }*/
}

bt_status_t bt_sink_srv_cap_stream_stop_scanning_broadcast_source(void)
{
    le_audio_log("[CAP][stream] stop scan broadcast source", 0); //MTK_COMMON: David - shrink log
    return ble_bap_stop_scanning_broadcast_source();
}

bt_status_t bt_sink_srv_cap_stream_broadcast_sync_periodic_advretising(bt_addr_t addr, uint8_t advertising_sid)
{
    if (g_default_bmr_scan_info.specified_bms) {

        le_audio_log("[CAP][stream] BROADCAST default addr:%02X:%02X:%02X:%02X:%02X:%02X", 6,
                     g_default_bmr_scan_info.bms_address[5],  g_default_bmr_scan_info.bms_address[4],  g_default_bmr_scan_info.bms_address[3],
                     g_default_bmr_scan_info.bms_address[2],  g_default_bmr_scan_info.bms_address[1],  g_default_bmr_scan_info.bms_address[0]);
        le_audio_log("[CAP][stream] BROADCAST syncing addr:%02X:%02X:%02X:%02X:%02X:%02X", 6,
                     addr.addr[5],  addr.addr[4],  addr.addr[3],
                     addr.addr[2],  addr.addr[1],  addr.addr[0]);
        if (0 == memcmp(g_default_bmr_scan_info.bms_address, addr.addr, sizeof(bt_bd_addr_t))) {
            return ble_bap_sync_broadcast_source_with_periodic_advertising(addr, advertising_sid);

            /*if (BT_STATUS_SUCCESS == ble_bap_sync_broadcast_source_with_periodic_advertising(addr, advertising_sid)) {
                memset(&g_default_bmr_scan_info.bms_address[0], 0, sizeof(bt_bd_addr_t));
                g_default_bmr_scan_info.specified_bms = false;
            }*/
        }
        return BT_STATUS_FAIL;
    } else {
        return ble_bap_sync_broadcast_source_with_periodic_advertising(addr, advertising_sid);
    }
}

bool bt_sink_srv_cap_stream_start_broadcast_reception(bt_handle_t sync_handle, bt_handle_t big_handle, uint8_t num_bis, uint8_t *bis_indices)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] start broadcast rcp, sync_hd:%x, big_hd:%x, num_bis:%d, bis_indices[0]:%x", 4,
                 sync_handle, big_handle, num_bis, bis_indices[0]);

    if ((num_bis == 1) && (bis_indices[0] == BIS_INDEX_INVALID)) {
        uint8_t bis_index = bt_sink_srv_cap_stream_get_bis_index_by_audio_location(g_default_bmr_scan_info.audio_channel_allocation, 0);
        if (bis_index != 0xFF) {
            bis_indices[0] = bis_index;
        } else {
            bis_indices[0] = ((g_default_bmr_scan_info.audio_channel_allocation == AUDIO_LOCATION_FRONT_RIGHT) ? 2 : 1);
        }
    }

    if (ble_bap_is_bis_valid(sync_handle, num_bis, bis_indices)) {
        //bt_sink_srv_cap_am_init(BROADCAST_MUSIC_MODE);
        bt_sink_srv_cap_stream_set_service_big(sync_handle, big_handle, num_bis, bis_indices);
        bt_sink_srv_cap_am_audio_start(CAP_AM_BROADCAST_MUSIC_MODE);
        return true;
    } else {
        ble_bap_reset_big_info(sync_handle);
        ble_bap_stop_syncing_to_periodic_advertising(sync_handle);
    }
    le_audio_log("[CAP][stream] ERROR! Unable to find BIS index", 0);

    return false;
}

bool bt_sink_srv_cap_stream_stop_broadcast_reception(void)
{
    le_audio_log("[CAP][stream] stop boradcast reception ", 0);

    bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();

    if (big_info->big_handle) {
        if (g_broadcast_keep_pa) {
            ble_bap_stop_syncing_to_periodic_advertising(big_info->sync_handle);
        }
        bt_sink_srv_cap_am_audio_stop(CAP_AM_BROADCAST_MUSIC_MODE);
        return true;
    }

    return false;
}

void bt_sink_srv_cap_stream_broadcast_enabling_response(bool is_accept)
{
    bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();

    if(is_accept) {
        /*Record current BIG configuration*/
        bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
        uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(bt_sink_srv_cap_stream_get_ble_link_with_cis_established(), cis_list);
        ble_bap_stream_config_info_t config_info = {0};

        if (ble_bap_get_broadcast_config_info(big_info->sync_handle, (ble_bap_stream_config_info_t*)&config_info)) {
            memcpy(&big_info->config_info.codec[0], &config_info.codec[0], AUDIO_CODEC_ID_SIZE);
            big_info->config_info.sampling_frequency = config_info.sampling_frequency;
            memcpy(&big_info->config_info.sdu_interval[0], &config_info.sdu_interval[0], SDU_INTERVAL_SIZE);
            big_info->config_info.frame_payload_length = config_info.frame_payload_length;
            big_info->config_info.is_stereo = config_info.is_stereo;
            big_info->config_info.metadata_length = config_info.metadata_length;
            if (NULL != big_info->config_info.metadata) {
                vPortFree(big_info->config_info.metadata);
                big_info->config_info.metadata = NULL;
            }
            if (config_info.metadata_length) {
                if (NULL == (big_info->config_info.metadata = pvPortMalloc(config_info.metadata_length))) {
                    configASSERT(0);
                } else {
                    memcpy(big_info->config_info.metadata, config_info.metadata, config_info.metadata_length);
                }
            }
        }

        if (cis_num) {
            le_audio_log("[CAP][stream] CIS exist, wait for CIS discneection then restart broadcast reception", 0);
        } else {
            ble_bap_start_broadcast_reception(big_info->sync_handle, big_info->big_handle, big_info->num_bis, big_info->bis_indices);
        }
    } else {
        ble_bap_stop_syncing_to_periodic_advertising(big_info->sync_handle);
        ble_bap_clear_big_info(big_info->big_handle);
        bt_sink_srv_cap_stream_clear_service_big(big_info->big_handle);
    }
}

void bt_sink_srv_cap_stream_broadcast_disabling_response(bool is_accept)
{
    bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();

    if (is_accept) {
        ble_bap_stop_broadcast_reception(big_info->big_handle);
        bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_BASE_BIG_TERMINATE_CFM, NULL);
#if defined (AIR_LE_AUDIO_BIS_ENABLE) && defined (MTK_RACE_CMD_ENABLE) && !defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        race_le_audio_notify_big_terminated_ind(0);
#endif
    }
}

bt_sink_srv_am_volume_level_out_t bt_sink_srv_cap_stream_get_broadcast_volume(void)
{
    return g_broadcast_volume;
}

bool bt_sink_srv_cap_stream_is_broadcast_mute(void)
{
    return g_broadcast_mute;
}

bool bt_sink_srv_cap_stream_set_broadcast_volume(bt_sink_srv_am_volume_level_out_t volume)
{
    if ((volume < AUD_VOL_OUT_MAX) && (volume != g_broadcast_volume)) {
        g_broadcast_volume = volume;
        bt_sink_srv_le_volume_set_volume(BT_SINK_SRV_LE_STREAM_TYPE_OUT, volume);
        return true;
    }
    return false;
}

bool bt_sink_srv_cap_stream_set_broadcast_mute(bool mute)
{
    if (bt_sink_srv_cap_stream_is_broadcast_streaming() && (mute != g_broadcast_mute)) {
        g_broadcast_mute = mute;
        bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, mute);
        return true;
    }
    return false;
}

bt_sink_srv_cap_stream_bmr_scan_info_t *bt_sink_srv_cap_stream_get_bmr_scan_info(void)
{
    return &g_default_bmr_scan_info;
}

void bt_sink_srv_cap_stream_reset_broadcast_state(void)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] reset broadcast state, sync_hd:%04X", 1, g_default_bmr_scan_info.sync_handle);
    ble_bap_reset_broadcast_state(g_default_bmr_scan_info.sync_handle);
}

bt_status_t bt_sink_srv_cap_stream_set_broadcast_code(uint8_t *broadcast_code)
{
    return ble_bap_set_broadcast_code(broadcast_code);
}

bt_status_t bt_sink_srv_cap_stream_get_broadcast_code(uint8_t *broadcast_code)
{
    return ble_bap_get_broadcast_code(broadcast_code);
}

bool bt_sink_srv_cap_stream_is_scanning_broadcast_source(void)
{
    return ble_bap_is_scanning_broadcast_source();
}

bool bt_sink_srv_cap_stream_restart_streaming(void)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_current_mode();
    bt_sink_srv_cap_am_mode restarting_mode = bt_sink_srv_cap_am_get_restarting_mode();
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] restart streaming, current_mode:%d, restarting_mode:%d", 2, mode, restarting_mode);
    if (CAP_INVALID_UINT8 != restarting_mode) {
        /*Restart streaming is proccessing*/
        return false;
    }
    if (mode <= CAP_AM_UNICAST_MUSIC_MODE_END) {
        bt_handle_t conn_handle = bt_sink_srv_cap_stream_get_service_ble_link();
        bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
        uint8_t path_direction[MAX_CIS_NUM] = {0x03, 0x03};
        uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(conn_handle, cis_list);

        //ble_bap_remove_cis_data_path(cis_num, cis_list, path_direction);
        /*ble_bap_ase_id_list_t ase_list = {0};
        ase_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(conn_handle);
        bt_handle_t cis_handle = BT_HANDLE_INVALID, pre_cis_handle = BT_HANDLE_INVALID;
        for (uint8_t i = 0; i < ase_list.num_of_ase; i++) {
            cis_handle = bt_sink_srv_cap_stream_get_cis_handle(conn_handle, ase_list.ase_id[i]);
            if (BT_HANDLE_INVALID != cis_handle && pre_cis_handle != cis_handle) {
                ble_bap_remove_cis_data_path(cis_handle, 0x03);
                pre_cis_handle = cis_handle;
            }
        }*/
        if (BT_STATUS_SUCCESS == ble_bap_remove_cis_data_path(cis_num, cis_list, path_direction)) {
            return bt_sink_srv_cap_am_audio_restart();
        }
    } else if (mode == CAP_AM_BROADCAST_MUSIC_MODE) {
        bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();
        if (NULL != big_info) {
            if (BT_STATUS_SUCCESS == ble_bap_remove_bis_data_path(big_info->big_handle, 0x03)) {
                return bt_sink_srv_cap_am_audio_restart();
            }
        }
    }
    return false;
}

bool bt_sink_srv_le_audio_switch(bool switch_on)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_current_mode();
    bt_sink_srv_cap_am_mode switch_suspending_mode = bt_sink_srv_cap_am_get_switch_suspending_mode();
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] audio switch, current_mode:%d, switch_suspending:%d", 2, mode, switch_suspending_mode);

    if (switch_on) {
        /*Resume DSP without changing ASE state and AM state*/
        g_cap_am_switch_suspending = false;
        if (mode == switch_suspending_mode) {
            return bt_sink_srv_cap_am_audio_switch(true);
        }
    } else {
        /*Suspend DSP without changing ASE state and AM state*/
        g_cap_am_switch_suspending = true;
        if (CAP_INVALID_UINT8 == mode) {
            /*LE audio is not streaming, just set suspending flag*/
            return false;
        }

        /*Remove ISO data path*/
        if (mode <= CAP_AM_UNICAST_MUSIC_MODE_END) {
            bt_handle_t conn_handle = bt_sink_srv_cap_stream_get_service_ble_link();
            bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
            uint8_t path_direction[MAX_CIS_NUM] = {0x03, 0x03};
            uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(conn_handle, cis_list);

            if (BT_STATUS_SUCCESS == ble_bap_remove_cis_data_path(cis_num, cis_list, path_direction)) {
                return bt_sink_srv_cap_am_audio_switch(false);
            }
        } else if (mode == CAP_AM_BROADCAST_MUSIC_MODE) {
            bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();
            if (NULL != big_info) {
                if (BT_STATUS_SUCCESS == ble_bap_remove_bis_data_path(big_info->big_handle, 0x03)) {
                    return bt_sink_srv_cap_am_audio_switch(false);
                }
            }
        }
    }
    return false;
}

void bt_sink_srv_cap_stream_set_all_cis_data_path(bt_handle_t connect_handle)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] set all cis path, hd:%d", 1, connect_handle);
    bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
    uint8_t path_id[MAX_CIS_NUM] = {2, 2}; //In MT7933, we set all path_id to 2 for using share memory mode
    uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(connect_handle, cis_list);
    ble_bap_set_cis_data_path(cis_num, cis_list, path_id);
}

void bt_sink_srv_cap_stream_remove_all_cis_data_path(bt_handle_t connect_handle)
{
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] rm all cis data path, hd:%d", 1, connect_handle);
    bt_handle_t cis_list[MAX_CIS_NUM] = {BT_HANDLE_INVALID, BT_HANDLE_INVALID};
#if 1//MTK_COMMON:Todo: we should check and set the correct direction; otherwise, the controller may rsp w/ cmd disallowed.
    uint8_t path_direction[MAX_CIS_NUM] = {0x02, 0x02};
#else
    uint8_t path_direction[MAX_CIS_NUM] = {0x03, 0x03};
#endif
    uint8_t cis_num = bt_sink_srv_cap_stream_get_cis_list(connect_handle, cis_list);

    ble_bap_remove_cis_data_path(cis_num, cis_list, path_direction);
}

bool bt_sink_srv_cap_stream_is_source_ase_only(bt_handle_t connect_handle)
{
    uint8_t link_index = 0, ase_index;
    ble_bap_ase_id_list_t ase_id_list = {0};

    if((link_index = bt_sink_srv_cap_get_link_index(connect_handle)) != g_cap_stream_service_info.max_link_num)
    {
        for(ase_index = 0; ase_index < CAP_MAX_ASE_NUM; ase_index++)
        {
            if(gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_state == ASE_STATE_STREAMING ||
                gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_state == ASE_STATE_ENABLING ||
                gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state == SUB_STATE_ENABLING_NOTIFYING ||
                gp_cap_stream_link_info[link_index].ase_info[ase_index].sub_state == SUB_STATE_ENABLING_RESPONDING)
            {
                ase_id_list.ase_id[ase_id_list.num_of_ase] = gp_cap_stream_link_info[link_index].ase_info[ase_index].ase_id;
                ase_id_list.num_of_ase++;
            }
        }
    }

    if (ase_id_list.num_of_ase == 1 &&
        bt_sink_srv_cap_stream_get_ase_direction(connect_handle, ase_id_list.ase_id[0]) == AUDIO_DIRECTION_SOURCE) {
        return true;
    }
    return false;
}

bool bt_sink_srv_cap_stream_start_unicast_streaming(bt_handle_t connect_handle)
{
    ble_bap_ase_id_list_t ase_list = bt_sink_srv_cap_stream_find_streaming_ase_id_list(connect_handle, false);
    uint8_t link_idx = bt_sink_srv_cap_get_link_index(connect_handle);
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_current_mode();
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] start unicast stream, hd:%d, mode:%d, link_idx:%d", 3,
        connect_handle, mode, link_idx);

    if (ase_list.num_of_ase) {
        /*Find last ASE direction to determine call/music mode*/
        uint8_t direction = bt_sink_srv_cap_stream_get_ase_direction(connect_handle, ase_list.ase_id[ase_list.num_of_ase - 1]);

        if (direction == AUDIO_DIRECTION_SINK) {
            bt_sink_srv_cap_am_audio_start(CAP_AM_UNICAST_MUSIC_MODE_START + link_idx);
        } else if (direction == AUDIO_DIRECTION_SOURCE) {
            bt_sink_srv_cap_am_audio_start(CAP_AM_UNICAST_CALL_MODE_START + link_idx);
        }
        return true;
    }
    return false;
}

void bt_sink_srv_cap_stream_keep_pa(bool is_keep_pa)
{
    if (!ble_bap_is_syncing_to_pa()){
        g_broadcast_keep_pa = is_keep_pa;
    }
    //MTK_COMMON: David - shrink log
    le_audio_log("[CAP][stream] cap stream keep pa, is_keep_pa:%x", 1, g_broadcast_keep_pa);
}

