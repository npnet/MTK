/* Copyright Statement:
 *
 * (C) 2021-2021  MediaTek Inc. All rights reserved.
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


/****************************************************************************
 *
 * HEADER FILES
 *
 ****************************************************************************/


// C library headers
#include <stdio.h>
#include <stdarg.h>

// fotav3 headers
#include "fota_osal.h"
#include "fota_log.h"

#ifdef MTK_FOTA_V3_BOOTLOADER_ENABLE
#include "hw_uart.h"
#endif


/****************************************************************************
 *
 * PUBLIC FUNCTIONS
 *
 ****************************************************************************/


#ifdef MTK_FOTA_V3_FREERTOS_ENABLE

void fota_log(char type, const char *fmt, ...)
{
    va_list ap;
    char    *str;

    if (type != FOTA_LOG_WARN && type != FOTA_LOG_ERRR)
        return;

    if (fmt[0] == '#' && fmt[1] == '\0')
        printf("#");
    else {
        va_start(ap, fmt);
        str = fota_malloc(vsnprintf(NULL, 0, fmt, ap) + 1);
        va_end(ap);

        if (str) {
            va_start(ap, fmt);
            (void)vsprintf(str, fmt, ap);
            va_end(ap);

            printf("%s\n", str);
        }

        fota_free(str);
    }
}

#endif


#ifdef MTK_FOTA_V3_BOOTLOADER_ENABLE

void fota_log(char type, const char *fmt, ...)
{
    va_list ap;

    if (type != FOTA_LOG_WARN && type != FOTA_LOG_ERRR)
        return;

    if (fmt[0] == '#' && fmt[1] == '\0')
        hw_uart_puts("#");
    else {
        va_start(ap, fmt);
        hw_uart_vprintf(fmt, ap);
        va_end(ap);
        hw_uart_puts("\n");
    }
}

#endif

