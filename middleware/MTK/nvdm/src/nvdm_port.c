/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
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

#ifdef MTK_NVDM_ENABLE
#include <string.h>

#ifndef MTK_NVDM_NO_FLASH_ENABLE
#include "hal_flash.h"
#endif
#include "syslog.h"
#include <assert.h>
#include "nvdm_port.h"

#if !defined (MTK_DEBUG_LEVEL_NONE)
extern log_control_block_t log_control_block_common;
void nvdm_port_log_notice(const char *message, ...)
{
    va_list ap;

    va_start(ap, message);
    vprint_module_log(&log_control_block_common, __FUNCTION__, __LINE__, PRINT_LEVEL_INFO, message, ap);
    va_end(ap);
}

void nvdm_port_log_info(const char *message, ...)
{
    va_list ap;

    va_start(ap, message);
    //vprint_module_log(&log_control_block_common, __FUNCTION__, __LINE__, PRINT_LEVEL_INFO, message, ap);
    vprint_module_log(&log_control_block_common, __FUNCTION__, __LINE__, PRINT_LEVEL_DEBUG, message, ap);
    va_end(ap);
}

void nvdm_port_log_error(const char *message, ...)
{
    va_list ap;

    va_start(ap, message);
    vprint_module_log(&log_control_block_common, __FUNCTION__, __LINE__, PRINT_LEVEL_ERROR, message, ap);
    va_end(ap);

    assert(0);
}
#else
void nvdm_port_log_notice(const char *message, ...)
{}
void nvdm_port_log_info(const char *message, ...)
{}
void nvdm_port_log_error(const char *message, ...)
{
    assert(0);
}
#endif

void nvdm_port_flash_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
#ifdef MTK_NVDM_NO_FLASH_ENABLE
    memcpy(buffer, (void *)address, length);
#else
    hal_flash_status_t status;

    status = hal_flash_read(address, buffer, length);
    if (status != HAL_FLASH_STATUS_OK) {
        NVDM_LOG_ERROR("hal_flash_read: address = 0x%08x, buffer = 0x%08x, length = %d, status = %d", address, (uint32_t)buffer, length, status);
    }
#endif
}

void nvdm_port_flash_write(uint32_t address, const uint8_t *buffer, uint32_t length)
{
#ifdef MTK_NVDM_NO_FLASH_ENABLE
    memcpy((void *)address, buffer, length);
#else
    hal_flash_status_t status;

    status = hal_flash_write(address, buffer, length);
    if (status != HAL_FLASH_STATUS_OK) {
        NVDM_LOG_ERROR("hal_flash_write: address = 0x%08x, buffer = 0x%08x, length = %d, status = %d", address, (uint32_t)buffer, length, status);
    }
#endif
}

/* erase unit is 4K large(which is size of PEB) */
void nvdm_port_flash_erase(uint32_t address)
{
#ifdef MTK_NVDM_NO_FLASH_ENABLE
    memset((void *)address, 0xFF, 4096);
#else
    hal_flash_status_t status;

    status = hal_flash_erase(address, HAL_FLASH_BLOCK_4K);
    if (status != HAL_FLASH_STATUS_OK) {
        NVDM_LOG_ERROR("hal_flash_erase: address = 0x%08x, status = %d", address, status);
    }
#endif
}

/* This macro defines NVDM using in OS context */
#define NVDM_PORT_USE_OS

#ifdef NVDM_PORT_USE_OS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef SemaphoreHandle_t nvdm_port_mutex_t;
static nvdm_port_mutex_t g_nvdm_mutex;
static TaskHandle_t g_task_handler;

void nvdm_port_mutex_creat(void)
{
    g_nvdm_mutex = xSemaphoreCreateMutex();

    if (g_nvdm_mutex == NULL) {
        NVDM_LOG_ERROR("nvdm_port_mutex_creat error\r\n");
    }
    NVDM_LOG_INFO("nvdm_port_mutex_creat successfully");
}

void nvdm_port_mutex_take(void)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        if (xSemaphoreTake(g_nvdm_mutex, portMAX_DELAY) == pdFALSE) {
            NVDM_LOG_ERROR("nvdm_port_mutex_take error\r\n");
        }
        NVDM_LOG_INFO("nvdm_port_mutex_take successfully");
    }
}

void nvdm_port_mutex_give(void)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        if (xSemaphoreGive(g_nvdm_mutex) == pdFALSE) {
            NVDM_LOG_ERROR("nvdm_port_mutex_give error\r\n");
        }
        NVDM_LOG_INFO("nvdm_port_mutex_give successfully");
    }
}

void *nvdm_port_malloc(uint32_t size)
{
    return pvPortMalloc(size);
}

void nvdm_port_free(void *pdata)
{
    vPortFree(pdata);
}

void nvdm_port_get_task_handler(void)
{
    g_task_handler = xTaskGetCurrentTaskHandle();
}

bool nvdm_port_query_task_handler(void)
{
    if (xTaskGetCurrentTaskHandle() != g_task_handler) {
        return false;
    }

    return true;
}

#ifdef SYSTEM_DAEMON_TASK_ENABLE
#include "system_daemon.h"

bool nvdm_port_send_queue(const void *p_item)
{
    BaseType_t ret;

    ret = system_daemon_send_message(SYSTEM_DAEMON_ID_NVDM, p_item);
    if (ret != pdPASS) {
        return false;
    }

    return true;
}
#else
bool nvdm_port_send_queue(const void *p_item)
{
    return false;
}
#endif

#else

#include "malloc.h"
void nvdm_port_mutex_creat(void)
{}

void nvdm_port_mutex_take(void)
{}

void nvdm_port_mutex_give(void)
{}

void *nvdm_port_malloc(uint32_t size)
{
    return malloc(size);
}

void nvdm_port_free(void *pdata)
{
    free(pdata);
}

void nvdm_port_get_task_handler(void)
{}

bool nvdm_port_query_task_handler(void)
{
    return true;
}

#endif

/* This macro defines max count of data items */
#define NVDM_PORT_DAT_ITEM_COUNT (256)

/* This macro defines size of PEB, normally it is size of flash block */
#define NVDM_PORT_PEB_SIZE (4096)

/* This macro defines max size of data item during all user defined data items.
 * 1. Must not define it greater than 2048 bytes.
 * 2. Define it as smaller as possible to enhance the utilization rate of NVDM region.
 * 2. Try your best to store small data less than 256 bytes.
 */
#define NVDM_PORT_MAX_DATA_ITEM_SIZE (2048)

/* This macro defines start address and PEB count of the NVDM region */
#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)

#include "flash_map.h"
#define NVDM_PORT_REGION_ADDRESS(pnum, offset) (NVDM_BASE + pnum * NVDM_PORT_PEB_SIZE + offset)
#define NVDM_PORT_REGION_PEB_COUNT    (NVDM_LENGTH / NVDM_PORT_PEB_SIZE)

#elif (PRODUCT_VERSION == 7933)

#ifdef MTK_NVDM_NO_FLASH_ENABLE
#define NVDM_FLASH_LENGTH ( NVDM_PORT_PEB_SIZE * 10 )
uint8_t ucNvramHeap[ NVDM_FLASH_LENGTH ];
#define NVDM_PORT_REGION_ADDRESS(pnum, offset) ((uint32_t)(ucNvramHeap + pnum * NVDM_PORT_PEB_SIZE + offset))
#else
#include "memory_map.h"
#define NVDM_FLASH_LENGTH    NVDM_LENGTH
#define NVDM_PORT_REGION_ADDRESS( pnum, offset ) ( NVDM_BASE + pnum * NVDM_PORT_PEB_SIZE + offset )
#endif
#define NVDM_PORT_REGION_PEB_COUNT    ( NVDM_FLASH_LENGTH / NVDM_PORT_PEB_SIZE )

#else

#include "memory_map.h"

#ifdef MTK_NVDM_MODEM_ENABLE
#include "nvdm_modem_port.h"
#else
#define ROM_NVDM_MODEM_LENGTH   0
#endif

#define NVDM_PORT_REGION_ADDRESS(pnum, offset) (ROM_NVDM_BASE - HAL_FLASH_BASE_ADDRESS + pnum * NVDM_PORT_PEB_SIZE + offset)
#define NVDM_PORT_REGION_PEB_COUNT    ((ROM_NVDM_LENGTH-ROM_NVDM_MODEM_LENGTH) / NVDM_PORT_PEB_SIZE)

#endif

/* This macro defines max length of group name of data item */
#define GROUP_NAME_MAX_LENGTH (16)

/* This macro defines max length of data item name of data item */
#define DATA_ITEM_NAME_MAX_LENGTH (32)

uint32_t nvdm_port_get_data_item_config(uint32_t *max_data_item_size,
                                        uint32_t *max_group_name_size,
                                        uint32_t *max_data_item_name_size)
{
    *max_data_item_size = NVDM_PORT_MAX_DATA_ITEM_SIZE;
    *max_group_name_size = GROUP_NAME_MAX_LENGTH;
    *max_data_item_name_size = DATA_ITEM_NAME_MAX_LENGTH;

    return NVDM_PORT_DAT_ITEM_COUNT;
}

uint32_t nvdm_port_get_peb_config(uint32_t *peb_count)
{
    *peb_count = NVDM_PORT_REGION_PEB_COUNT;

    return NVDM_PORT_PEB_SIZE;
}

uint32_t nvdm_port_get_peb_address(int32_t pnum, int32_t offset)
{
    return NVDM_PORT_REGION_ADDRESS(pnum, offset);
}

void nvdm_port_poweroff_time_set(void)
{}
void nvdm_port_poweroff(uint32_t poweroff_time)
{}

#endif

