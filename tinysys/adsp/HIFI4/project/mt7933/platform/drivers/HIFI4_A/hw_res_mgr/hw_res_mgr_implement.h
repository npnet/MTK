/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
#ifndef _HW_RES_MGR_IMPLEMENT_H_
#define _HW_RES_MGR_IMPLEMENT_H_

/* user list */
enum DSP_HW_USER_LIST {
    DSP_HW_USER_TOP_CTRL = 0,
    DSP_HW_USER_VA_TASK,
    DSP_HW_USER_RES_MGR,
#ifdef CFG_AUDIO_PLAYBACK_SUPPORT
    DSP_HW_USER_PLAYBACK_TASK,
#ifdef CFG_PROMPT_SOUND_ENABLE
    DSP_HW_USER_VP_PLAYBACK,
#endif
#endif
    DSP_HW_USER_NUM,
};

/* resource list */
enum DSP_HW_RES_LIST {
    DSP_HW_RES_SPM_REQ = 0,
    DSP_HW_RES_CLK,
    DSP_HW_RES_SPM_INT,
    DSP_HW_RES_PSRAM_REQ,
    DSP_HW_RES_NUM,
};

/* List resource from low to high request */
/* 0 is the lowest request */
enum DSP_SYS_HW_REQ {
    DSP_SYS_HW_NONE = 0,
    DSP_SYS_HW_26M,
    DSP_SYS_HW_INFRA,
    DSP_SYS_HW_PLL,
    DSP_SYS_HW_DRAM,
    DSP_SYS_HW_REQ_NUM,
};

enum DSP_PSRAM_HW_REQ {
    DSP_PSRAM_NONEED = 0,
    DSP_PSRAM_NEED,
};

int hw_res_implement_init(void);
int hw_res_implement_uninit(void);

#endif
