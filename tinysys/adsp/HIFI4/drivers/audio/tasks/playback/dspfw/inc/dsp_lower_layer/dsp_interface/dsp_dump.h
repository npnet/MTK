/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */


/******************************************************************************
 * Constant define
 ******************************************************************************/
#define DUMPDATA_MASK_WORD_NO       (2)
#define NO_OF_BITS_IN_A_WORD        (32)

/******************************************************************************
 * Enumerations
 ******************************************************************************/
typedef enum
{
    _RESERVED                   = 0,
    SOURCE_IN1                  = 1,
    SOURCE_IN2                  = 2,
    SOURCE_IN3                  = 3,
    SOURCE_IN4                  = 4,
    SOURCE_IN5                  = 5,
    SINK_OUT1                   = 6,
    SINK_OUT2                   = 7,
    SINK_SUBPATH_OUT            = 8,
    AUDIO_CODEC_IN_LENGTH       = 9,

    VOICE_TX_MIC_0              = 10,
    VOICE_TX_MIC_1              = 11,
    VOICE_TX_MIC_2              = 12,
    VOICE_TX_MIC_3              = 13,
    VOICE_TX_REF                = 14,
    VOICE_TX_NR_OUT             = 15,
    VOICE_TX_CPD_IN             = 16,
    VOICE_TX_CPD_OUT            = 17,
    VOICE_TX_OUT                = 18,

    VOICE_RX_PLC_IN             = 19,
    VOICE_RX_PLC_INFO           = 20,
    VOICE_RX_PLC_OUT            = 21,
    VOICE_RX_NR_IN              = 22,
    VOICE_RX_NR_OUT             = 23,
    VOICE_RX_CPD_IN             = 24,
    VOICE_RX_CPD_OUT            = 25,
    VOICE_RX_OUT                = 26,

    VOICE_VC_IN1                = 27,
    VOICE_VC_IN2                = 28,
    VOICE_VC_RESULT             = 29,

    PROMPT_VP_PATTERN           = 30,
    PROMPT_VP_OUT               = 31,
    PROMPT_RT_PATTERN           = 32,
    PROMPT_RT_OUT               = 33,

    AUDIO_CODEC_IN              = 34,
    AUDIO_SOURCE_IN_L           = 35,
    AUDIO_SOURCE_IN_R           = 36,
    AUDIO_INS_IN_L              = 37,
    AUDIO_INS_IN_R              = 38,
    AUDIO_INS_OUT_L             = 39,
    AUDIO_INS_OUT_R             = 40,
    AUDIO_ENHANCEMENT_IN_L      = 41,
    AUDIO_ENHANCEMENT_IN_R      = 42,
    AUDIO_ENHANCEMENT_OUT_L     = 43,
    AUDIO_ENHANCEMENT_OUT_R     = 44,
    AUDIO_PEQ_IN_L              = 45,
    AUDIO_PEQ_IN_R              = 46,
    AUDIO_PEQ_OUT_L             = 47,
    AUDIO_PEQ_OUT_R             = 48,
    AUDIO_PEQ2_IN_L             = 49,
    AUDIO_PEQ2_IN_R             = 50,
    AUDIO_PEQ2_OUT_L            = 51,
    AUDIO_PEQ2_OUT_R            = 52,
    AUDIO_CPD_IN_L              = 53,
    AUDIO_CPD_IN_R              = 54,
    AUDIO_CPD_OUT_L             = 55,
    AUDIO_CPD_OUT_R             = 56,

    AUDIO_SOUNDBAR_INPUT        = 57,
    AUDIO_SOUNDBAR_TX           = 58,

    AUDIO_WOOFER_RX             = 59,
    AUDIO_WOOFER_UPSAMPLE_8K    = 60,
    AUDIO_WOOFER_PLC_OUT        = 61,
    AUDIO_WOOFER_UPSAMPLE_16K   = 62,
    AUDIO_WOOFER_CPD_OUT        = 63,

    DSP_DATADUMP_MAX_BIT        = 63,
} DSP_DATADUMP_MASK_BIT;

/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
extern void LOG_AUDIO_DUMP(U8 *audio, U32 audio_size, DSP_DATADUMP_MASK_BIT dumpID);
extern U32 AudioDumpMask[DUMPDATA_MASK_WORD_NO];
extern U32 AudioDumpDevice;
#ifdef MTK_AUDIO_DUMP_BY_SPDIF_ENABLE
extern void audio_dump_task_handler(uint32_t arg);
extern void audio_dump_init(void);
#endif
