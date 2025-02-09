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

/**
 * This file contains the root of all CLI commands. They are divided into
 * three modes: NORMAL, TEST, EXAMPLE.
 *
 * Normal and test are enabled by default. TEST mode commands can be turned
 * off by setting MTK_CLI_TEST_MODE_ENABLE to 'n' in feature.mk.
 */

/****************************************************************************
 *
 * Header files.
 *
 ****************************************************************************/

#if defined(MTK_MINICLI_ENABLE)


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "cli_def.h"
#include "cli_cmds.h"

#include <toi.h>


extern void cli_cmds_init(cli_t *cli);


int bl_getchar(void)
{
    int c;
    extern int hw_uart_getc(void);
    
    while ((c = hw_uart_getc()) == -1) ;
    return c;
}

int bl_putchar(int c)
{
    static int last;
    
    // todo: use hal_uart API
    extern void hw_uart_putc(char c);
    
    if (c == '\n' && last != '\r')
        hw_uart_putc('\r');
    last = c;

    hw_uart_putc((char)c);
    return 0;
}


static cli_t _cli_cb = {
    .state  = 1,
    .echo   = 0,
    .get    = bl_getchar,
    .put    = bl_putchar,
};


/****************************************************************************
 * Private Functions
 ****************************************************************************/


#define HISTORY_LINE_MAX    (128)
#define HISTORY_LINES       (5)


static char s_history_lines[ HISTORY_LINES ][ HISTORY_LINE_MAX ];
static char *s_history_ptrs[ HISTORY_LINES ];

static char s_history_input[ HISTORY_LINE_MAX ];
static char s_history_parse_token[ HISTORY_LINE_MAX ];


/****************************************************************************
 * Public Functions
 ****************************************************************************/


void cli_create(void)
{
    cli_history_t *hist = &_cli_cb.history;
    int i;

    for (i = 0; i < HISTORY_LINES; i++) {
        s_history_ptrs[i] = s_history_lines[i];
    }
    hist->history           = &s_history_ptrs[0];

    hist->input             = s_history_input;
    hist->parse_token       = s_history_parse_token;
    hist->history_max       = HISTORY_LINES;
    hist->line_max          = HISTORY_LINE_MAX;
    hist->index             = 0;
    hist->position          = 0;
    hist->full              = 0;

    _cli_cb.state           = 1,
    cli_cmds_init(&_cli_cb);
    cli_init(&_cli_cb);
}


void cli_forever(void)
{
    while (1) {
        cli_task();
    }
}


#endif /* #if defined(MTK_MINICLI_ENABLE) */

