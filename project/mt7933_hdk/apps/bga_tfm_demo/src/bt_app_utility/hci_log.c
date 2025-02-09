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

//#include "uart.h"
#include "hal_platform.h"
#include "hal_uart.h"
#include "hci_log.h"
#include <stdio.h>
#include <string.h>

#define MTK_HCI_CONSOLE_MIX_ENABLE
#ifdef MTK_HCI_CONSOLE_MIX_ENABLE
//#include "mt7687.h"
#define HCI_MAGIC_HI    0xab
#define HCI_MAGIC_LO    0xcd
#define BT_HCILOG_PORT  HAL_UART_0
#else
#define BT_HCILOG_PORT  HAL_UART_1
#endif
extern int log_write(char *buf, int len);

#ifdef BLE_THROUGHPUT
// only output part hci log data to prevent it influencing throughput.
#define HCI_LOG_LEN (15)      // HEAD:16, payload: frame_num 1, frame header: 5, payload len:2
static unsigned char g_hci_log_buf[2048];
static unsigned char g_gatt_omit_buf[28];
static unsigned char *hci_log_gatt_part_data_omit(int32_t buf_len, unsigned char *buf, uint32_t *out_len)
{
    if (buf_len > HCI_LOG_LEN) {
        memcpy(g_gatt_omit_buf, buf, HCI_LOG_LEN);
        g_gatt_omit_buf[2] = HCI_LOG_LEN - 4;
        g_gatt_omit_buf[3] = 0;
        g_gatt_omit_buf[4] = HCI_LOG_LEN - 8;
        g_gatt_omit_buf[5] = 0;
        *out_len = HCI_LOG_LEN;
        return g_gatt_omit_buf;
    } else {
        memcpy(g_hci_log_buf, buf, buf_len);
        *out_len = buf_len;
        return g_hci_log_buf;
    }
}
#endif

static int32_t hci_log(unsigned char type, unsigned char *buf, int32_t length)
{
    //    int write_ret = 0;
    unsigned char head[8] = {0xAB, 0xCD, type, (uint8_t)((length & 0xff00) >> 8), (uint8_t)(length & 0xff)};
    unsigned char cache[8] = {0};
    memcpy(cache, buf - 8, 8);
    memcpy(buf - 5, head, 5);
#ifdef MTK_HCI_CONSOLE_MIX_ENABLE
    //    __disable_irq();
    log_write((char *)buf - 5, length + 5);
    //    write_ret = log_write((char *)buf - 5, length + 5);
#else
    log_write((char *)buf - 3, length + 3);
    //    write_ret = log_write((char *)buf - 3, length + 3);
#endif

#ifdef MTK_HCI_CONSOLE_MIX_ENABLE
    //   __enable_irq();
#endif
    memcpy(buf - 8, cache, 8);
    return length;
}

int32_t hci_log_cmd(unsigned char *buf, int32_t length)
{
    return hci_log(HCI_COMMAND, buf, length);
}

int32_t hci_log_event(unsigned char *buf, int32_t length)
{
    return hci_log(HCI_EVENT, buf, length);
}

int32_t hci_log_acl_out(unsigned char *buf, int32_t length)
{
#ifdef BLE_THROUGHPUT
    uint32_t out_len;
    unsigned char *omit_buf = hci_log_gatt_part_data_omit(length, buf, &out_len);
    return hci_log(HCI_ACL_OUT, omit_buf, out_len);
#else
    return hci_log(HCI_ACL_OUT, buf, length);
#endif
}
int32_t hci_log_acl_in(unsigned char *buf, int32_t length)
{
    return hci_log(HCI_ACL_IN, buf, length);
}
