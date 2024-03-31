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

#include <stdio.h>
#include <stdlib.h>
#include "syslog.h"
#include "syslog_cli.h"
#include <string.h>
/****************************************************************************
 *
 * Actual CLI API
 *
 ****************************************************************************/

uint8_t syslog_cli_set_filter(uint8_t len, char *param[])
{
#if !defined (MTK_DEBUG_LEVEL_NONE)

    int i = 0;

    if (len < 3 || len % 3) {
        printf("required parameters: <module_name> <log_switch> <print_level>\r\n");
        printf("<log_switch>   := on | off\r\n");
        printf("<print_level>  := debug | info | warning | error\r\n");
        return 1;
    }

    while (i < len) {
        if (syslog_set_filter(param[i], param[i + 1], param[i + 2], (i == len - 3))) {
            printf("invalid %s %s %s\n", param[i], param[i + 1], param[i + 2]);
            return 2;
        }
        i += 3;
    }

    return 0;

#else /* #if !defined (MTK_DEBUG_LEVEL_NONE) */

    printf("syslog not supported. check MTK_DEBUG_LEVEL in project's feature.mk\n");
    return 0;

#endif /* #if !defined (MTK_DEBUG_LEVEL_NONE) */

}

uint8_t syslog_cli_show_config(uint8_t len, char *param[])
{
#if !defined (MTK_DEBUG_LEVEL_NONE)

    syslog_config_t   config;
    int               i   = 0;

    syslog_get_config(&config);

    printf("%s\t%s\t%s\r\n", "module", "on/off", "level");
    printf("%s\t%s\t%s\r\n", "------", "------", "-----");

    while (config.filters && config.filters[i] != NULL) {
        printf("%s\t%s\t%s\r\n",
               config.filters[i]->module_name,
               log_switch_to_str(config.filters[i]->log_switch),
               print_level_to_str(config.filters[i]->print_level));
        i++;
    }

    return 0;

#else /* #if !defined (MTK_DEBUG_LEVEL_NONE) */

    printf("syslog not supported. check MTK_DEBUG_LEVEL in project's feature.mk\n");
    return 0;

#endif /* #if !defined (MTK_DEBUG_LEVEL_NONE) */

}


uint8_t syslog_set_format(uint8_t len, char *param[])
{
#if !defined (MTK_DEBUG_LEVEL_NONE)
#define BIT_STR "time  ", "module", "level ", "func  ", "line  ", "stm   ", "id    ", "sntp  "
#define PRINT_BITS(num)                                                 \
char* str[8] = {BIT_STR};                                               \
for(int i = 7 ; i >= 0 ; i--) printf("%s ", str[i]);                    \
printf("\n");                                                           \
for(int i = 7 ; i >= 0 ; i--) printf("%d      ", check_bit(num, i)?1:0);\
printf("\n");


    if (len < 1) {
        printf("Set format  : log fmt [module] [format]\n");
        printf("Check format: log fmt [module]\n");
        return 0;
    }

    syslog_config_t   config;
    int               i   = 0;

    syslog_get_config(&config);
    uint8_t format;
    while (config.filters && config.filters[i] != NULL) {
        if (!strcmp(param[0], config.filters[i]->module_name)) {
            if (len == 1) {
                format = config.filters[i]->module_format;
                PRINT_BITS(format);
            } else if (len == 2) {
                format = (uint8_t)(strtoul(param[1], NULL, 16));
                config.filters[i]->module_format = format;
                PRINT_BITS(format);
            }
            break;
        } else {
            i++;
            continue;
        }
    }
#else /* #if !defined (MTK_DEBUG_LEVEL_NONE) */

    printf("syslog not supported. check MTK_DEBUG_LEVEL in project's feature.mk\n");
    return 0;
	
#endif /* #if !defined (MTK_DEBUG_LEVEL_NONE) */
    return 0;
}

#if defined(MTK_MINICLI_ENABLE)
cmd_t syslog_cli[] = {
    { "set",    "setup filter",  syslog_cli_set_filter,   NULL },
#if !defined (MTK_DEBUG_LEVEL_NONE)
#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
    { "switch",  "setup filter",  syslog_switch_cmds,  NULL },
#endif /* #if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE) */
#endif /* #if !defined (MTK_DEBUG_LEVEL_NONE) */
    { "fmt",  "set format ut",  syslog_set_format,  NULL },
    { NULL,     NULL,            NULL,                    NULL }
};
#endif /* #if defined(MTK_MINICLI_ENABLE) */

