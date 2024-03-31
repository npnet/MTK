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


//-
#include <string.h>

#include "dsp_sdk.h"

#include "dsp_feature_interface.h"
#include "dsp_stream_connect.h"
#include "dsp_audio_process.h"
#ifdef AIR_BT_HFP_CVSD_ENABLE
#include "cvsd_enc_interface.h"
#include "cvsd_dec_interface.h"
#endif
#ifdef AIR_BT_HFP_MSBC_ENABLE
#include "msbc_enc_interface.h"
#include "msbc_dec_interface.h"
#endif
//#include "dprt_rt.h"
#ifdef MTK_BT_A2DP_SBC_ENABLE
#include "sbc_interface.h"
#endif /* MTK_BT_A2DP_SBC_ENABLE */
#ifdef MTK_BT_A2DP_AAC_ENABLE
#include "aac_dec_interface.h"
#endif /* MTK_BT_A2DP_AAC_ENABLE */
//#include "compander_interface.h"

#ifdef AIR_VOICE_NR_ENABLE
#include "aec_nr_interface.h"
#ifdef MTK_3RD_PARTY_NR
#include "tx_eq_interface.h"
#endif
#if defined(AIR_EC120_ENABLE)
#include "ec120_portable.h"
#endif
#endif
#ifdef CLKSKEW_READY
#include "clk_skew.h"
#endif
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#include "peq_interface.h"
#endif
#ifdef MTK_LINEIN_INS_ENABLE
#include "ins_interface.h"
#endif
#ifdef MTK_WWE_ENABLE
#include "wwe_interface.h"
#endif

#include "dsp_vendor_codec_sdk.h"

//#if defined(MTK_BT_A2DP_ENABLE)|| defined(MTK_BT_HFP_ENABLE) || defined(AIR_BT_CODEC_BLE_ENABLED) || defined(MTK_PROMPT_SOUND_ENABLE)
#if defined(MTK_BT_A2DP_ENABLE)|| defined(MTK_BT_HFP_ENABLE) || defined(AIR_BT_CODEC_BLE_ENABLED)
#include "ch_select_interface.h"
#endif
//#include "mute_smooth_interface.h"

#ifdef MTK_VOICE_AGC_ENABLE
#include "agc_interface.h"
#endif

#ifdef MTK_SENSOR_SOURCE_ENABLE
#include "gsensor_processing.h"
#endif
#ifdef MTK_AUDIO_PLC_ENABLE
#include "audio_plc_interface.h"
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "lc3_enc_interface.h"
#include "lc3_dec_interface.h"
#endif

#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
#include "audio_loopback_test_interface.h"
#endif

#ifdef MTK_CELT_DEC_ENABLE
#include "celt_dec_interface.h"
#endif
#ifdef AIR_CELT_ENC_ENABLE
#include "celt_enc_interface.h"
#endif
#ifdef AIR_SOFTWARE_SRC_ENABLE
#include "sw_src_interface.h"
#endif
#ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
#include "clk_skew_sw.h"
#endif
#ifdef AIR_SOFTWARE_GAIN_ENABLE
#include "sw_gain_interface.h"
#endif
#ifdef AIR_SOFTWARE_MIXER_ENABLE
#include "sw_mixer_interface.h"
#endif
#ifdef AIR_SOFTWARE_BUFFER_ENABLE
#include "sw_buffer_interface.h"
#endif
#ifdef AIR_AFC_ENABLE
#include "afc_interface.h"
#endif
#ifdef AIR_LD_NR_ENABLE
#include "ld_nr_interface.h"
#endif
#ifdef AIR_AT_AGC_ENABLE
#include "at_agc_interface.h"
#endif
#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "lc3_enc_branch_interface.h"
#include "lc3_dec_interface_v2.h"
#endif
#endif

#ifdef AIR_FIXED_RATIO_SRC
#include "src_fixed_ratio_interface.h"
#endif



////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct stream_codec_sample_instance_u
{
    uint8_t scratch_memory[128];
    uint8_t output[512];
    bool memory_check;
    bool reset_check;
} stream_codec_sample_instance_t, *stream_codec_sample_instance_ptr_t;

typedef struct stream_function_sample_instance_u
{
    uint32_t coefficient_size;
    uint8_t filter[32];
    int16_t buffer_l[512];
    int16_t buffer_r[512];
} stream_function_sample_instance_t, *stream_function_sample_instance_ptr_t;


////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define CODEC_SAMPLE_MEMORY_SIZE        sizeof(stream_codec_sample_instance_t)
#define CODEC_OUTPUT_SIZE               1024

#define FUNCTION_SAMPLE_MEMORY_SIZE     sizeof(stream_function_sample_instance_t)




////////////////////////////////////////////////////////////////////////////////
// Function Prototypes /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool stream_codec_decoder_sample_initialize(void *para);
bool stream_codec_decoder_sample_process(void *para);
bool stream_function_sample_initialize(void *para);
bool stream_function_sample_process(void *para);
void stream_feature_sample_open  (void);
void stream_feature_sample_close (void);
uint32_t codec_decoder_sample_api(uint8_t *pattern_pointer, int16_t *l_pointer, int16_t *r_pointer, uint32_t input_length);
uint32_t function_sample_api(stream_function_sample_instance_ptr_t instance_ptr, int16_t *input_l_pointer, int16_t *input_r_pointer, int16_t *output_l_pointer, int16_t *output_r_pointer, uint32_t length);


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*===========================================================================================*/
// Codec blocks
// CodecOutSize must have value
stream_feature_table_t stream_feature_table[DSP_FEATURE_MAX_NUM] =
{
    /*====================================================================================    stream feature codec    ====================================================================================*/
    /*feature_type,                                              memory_size,          codec_output_size,                               *initialize_entry,                                  *process_entry*/
    {CODEC_PCM_COPY,                                                       0,                       2048,                      stream_pcm_copy_initialize,                         stream_pcm_copy_process},/*0x00 CODEC_PCM_COPY               */ /* Only copy in_ptr memory to out_ptr memory */
#ifdef AIR_BT_HFP_CVSD_ENABLE
    {CODEC_DECODER_CVSD,                            DSP_CVSD_DECODER_MEMSIZE,  480+DSP_SIZE_FOR_CLK_SKEW,            stream_codec_decoder_cvsd_initialize,               stream_codec_decoder_cvsd_process},/*0x01 CVSD_Decoder,    */   // 120(NB), 240(WB)
#else
    {CODEC_DECODER_CVSD,                                                   0,                       0xFF,                                            NULL,                                            NULL},/*0x01 CVSD_Decoder,    */   // 120(NB), 240(WB)
#endif
#ifdef AIR_BT_HFP_MSBC_ENABLE
    {CODEC_DECODER_MSBC,                            DSP_MSBC_DECODER_MEMSIZE,  480+DSP_SIZE_FOR_CLK_SKEW,            stream_codec_decoder_msbc_initialize,               stream_codec_decoder_msbc_process},/*0x02 CODEC_DECODER_MSBC,      */
#else
    {CODEC_DECODER_MSBC,                                                   0,                       0xFF,                                            NULL,                                            NULL},/*0x02 CODEC_DECODER_MSBC,      */
#endif
#ifdef MTK_BT_A2DP_SBC_ENABLE
    {CODEC_DECODER_SBC,                                DSP_SBC_CODEC_MEMSIZE, 4096+DSP_SIZE_FOR_CLK_SKEW,             stream_codec_decoder_sbc_initialize,                stream_codec_decoder_sbc_process},/*0x03 CODEC_DECODER_SBC,       */
#else
    {CODEC_DECODER_SBC,                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x03 CODEC_DECODER_SBC,       */
#endif
#ifdef MTK_BT_A2DP_AAC_ENABLE
    {CODEC_DECODER_AAC,                              DSP_AAC_DECODER_MEMSIZE, 4096+DSP_SIZE_FOR_CLK_SKEW,             stream_codec_decoder_aac_initialize,                stream_codec_decoder_aac_process},/*0x04 CODEC_DECODER_AAC,       */
#else
    {CODEC_DECODER_AAC,                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x04 CODEC_DECODER_AAC,       */
#endif
    {CODEC_DECODER_MP3,                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x05 _Reserved,       */
    {CODEC_DECODER_EC,                                                     0,                       0xFF,                                            NULL,                                            NULL},/*0x06 _Reserved,       */
    {CODEC_DECODER_UART,                                                   0,                       0xFF,                                            NULL,                                            NULL},/*0x07 _Reserved,       */
    {CODEC_DECODER_UART16BIT,                                              0,                       0xFF,                                            NULL,                                            NULL},/*0x08 _Reserved,       */
#ifdef AIR_BT_CODEC_BLE_ENABLED
    {CODEC_DECODER_LC3,                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x0E CODEC_DECODER_LC3,       */
#endif
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x0F _Reserved,       */
#ifdef AIR_BT_HFP_CVSD_ENABLE
    {CODEC_ENCODER_CVSD,                            DSP_CVSD_ENCODER_MEMSIZE,                        240,            stream_codec_encoder_cvsd_initialize,               stream_codec_encoder_cvsd_process},/*0x10 CVSD_Encoder,    */   // 60(NB), 120(WB)
#else
    {CODEC_ENCODER_CVSD,                                                   0,                       0xFF,                                            NULL,                                            NULL},/*0x10 CVSD_Encoder,    */   // 60(NB), 120(WB)
#endif
#ifdef AIR_BT_HFP_MSBC_ENABLE
    {CODEC_ENCODER_MSBC,                            DSP_MSBC_ENCODER_MEMSIZE,                        240,            stream_codec_encoder_msbc_initialize,               stream_codec_encoder_msbc_process},/*0x11 CODEC_ENCODER_MSBC,      */
#else
    {CODEC_ENCODER_MSBC,                                                   0,                       0xFF,                                            NULL,                                            NULL},/*0x11 CODEC_ENCODER_MSBC,      */
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
    {CODEC_ENCODER_LC3,                                                    0,  DSP_LC3_ENCODER_OUT_BUF_SIZE+DSP_SIZE_FOR_CLK_SKEW,             stream_codec_encoder_lc3_initialize,                stream_codec_encoder_lc3_process},/*0x13 CODEC_ENCODER_LC3,       */
#else
    {CODEC_ENCODER_LC3,                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x13 CODEC_ENCODER_LC3,       */
    {CODEC_ENCODER_LC3_BRANCH,                                             0,                       0xFF,                                            NULL,                                            NULL},/*0x14 CODEC_ENCODER_LC3,       */
#endif
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x15 _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x16 _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x17 _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x18 _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x19 _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x1A _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x1B _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x1C _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x1D _Reserved,       */
    {0,                                                                    0,                       0xFF,                                            NULL,                                            NULL},/*0x1E _Reserved,       */
    /*====================================================================================    stream feature function    ====================================================================================*/
    /*feature_type,                                              memory_size,      0(Must equal to zero),                               *initialize_entry,                                  *process_entry*/
#ifdef MTK_HWSRC_IN_STREAM
    {DSP_SRC,                                                DSP_SRC_MEMSIZE,                          0,                  stream_function_src_initialize,                     stream_function_src_process},/*0x1F  DSP_SRC,       */
#else
    {DSP_SRC,                                                              0,                          0,                                            NULL,                                            NULL},/*0x1F  DSP_SRC,       */
#endif
    {FUNC_END,                                                             0,                          0,                  stream_function_end_initialize,                     stream_function_end_process},/*0x20 FUNC_END,                */
#ifdef AIR_VOICE_DRC_ENABLE
    {FUNC_RX_WB_DRC,                                                       0,                          0,      stream_function_drc_voice_rx_wb_initialize,            stream_function_drc_voice_rx_process},/*0x21 FUNC_RX_WB_DRC,          */
    {FUNC_RX_NB_DRC,                                                       0,                          0,      stream_function_drc_voice_rx_nb_initialize,            stream_function_drc_voice_rx_process},/*0x22 FUNC_RX_NB_DRC,          */
    {FUNC_TX_WB_DRC,                                                       0,                          0,      stream_function_drc_voice_tx_wb_initialize,            stream_function_drc_voice_tx_process},/*0x23 FUNC_TX_WB_DRC,       */
    {FUNC_TX_NB_DRC,                                                       0,                          0,      stream_function_drc_voice_tx_nb_initialize,            stream_function_drc_voice_tx_process},/*0x24 FUNC_TX_NB_DRC,       */
#else
    {FUNC_RX_WB_DRC,                                                       0,                          0,                                            NULL,                                             NULL},/*0x21 FUNC_RX_WB_DRC,    */
    {FUNC_RX_NB_DRC,                                                       0,                          0,                                            NULL,                                             NULL},/*0x22 FUNC_RX_NB_DRC,       */
    {FUNC_TX_WB_DRC,                                                       0,                          0,                                            NULL,                                             NULL},/*0x23 FUNC_TX_WB_DRC,      */
    {FUNC_TX_NB_DRC,                                                       0,                          0,                                            NULL,                                             NULL},/*0x24 FUNC_TX_NB_DRC,       */
#endif
#ifdef AIR_VOICE_NR_ENABLE
    {FUNC_RX_NR,                                                           0,                          0,               stream_function_aec_nr_initialize,                      stream_function_nr_process},/*0x25 FUNC_RX_NR,                */
#if defined(MTK_3RD_PARTY_NR)
    {FUNC_TX_NR,                                              IGO_NR_MEMSIZE,                          0,               stream_function_aec_nr_initialize,                     stream_function_aec_process},/*0x26 FUNC_TX_NR,                */
#else
    {FUNC_TX_NR,                                                           0,                          0,               stream_function_aec_nr_initialize,                     stream_function_aec_process},/*0x26 FUNC_TX_NR,                */
#endif
#else
    {FUNC_RX_NR,                                                           0,                          0,                                            NULL,                                             NULL},/*0x25 FUNC_RX_NR,                */
    {FUNC_TX_NR,                                                           0,                          0,                                            NULL,                                             NULL},/*0x26 FUNC_TX_NR,                */
#endif
#ifdef AIR_BT_CLK_SKEW_ENABLE
#ifdef ENABLE_2A2D_TEST
    {FUNC_CLK_SKEW_UL,     DSP_CLK_SKEW_MEMSIZE*5+DSP_CLK_SKEW_TEMP_BUF_SIZE,                          0,    stream_function_clock_skew_uplink_initialize,       stream_function_clock_skew_uplink_process},/*0x27 FUNC_CLK_SKEW_UL,        */
#else
    {FUNC_CLK_SKEW_UL,     DSP_CLK_SKEW_MEMSIZE*3+DSP_CLK_SKEW_TEMP_BUF_SIZE,                          0,    stream_function_clock_skew_uplink_initialize,       stream_function_clock_skew_uplink_process},/*0x27 FUNC_CLK_SKEW_UL,        */
#endif
    {FUNC_CLK_SKEW_DL,     DSP_CLK_SKEW_MEMSIZE*4+DSP_CLK_SKEW_TEMP_BUF_SIZE,                          0,  stream_function_clock_skew_downlink_initialize,     stream_function_clock_skew_downlink_process},/*0x28 FUNC_CLK_SKEW_DL,        */
#else
    {FUNC_CLK_SKEW_UL,                                                     0,                          0,                                            NULL,                                            NULL},/*0x27 FUNC_CLK_SKEW_UL,        */
    {FUNC_CLK_SKEW_DL,                                                     0,                          0,                                            NULL,                                            NULL},/*0x28 FUNC_CLK_SKEW_DL,        */
#endif
#ifdef CFG_DSP_GAIN_CONTROL_ENABLE
    {FUNC_MIC_SW_GAIN,                                                     8,                          0,                 stream_function_gain_initialize,                    stream_function_gain_process},/*0x29 FUNC_MIC_SW_GAIN,        */
#else
    {FUNC_MIC_SW_GAIN,                                                     8,                          0,                                            NULL,                                            NULL},/*0x29 FUNC_MIC_SW_GAIN,        */
#endif
#ifdef AIR_VOICE_PLC_ENABLE
    {FUNC_PLC,                                         DSP_VOICE_PLC_MEMSIZE,                          0,                  stream_function_plc_initialize,                     stream_function_plc_process},/*0x2A FUNC_PLC,                */
#else
    {FUNC_PLC,                                                             0,                          0,                                            NULL,                                            NULL},/*0x2A FUNC_PLC,   */
#endif
#ifdef AIR_BT_CLK_SKEW_ENABLE
    {FUNC_CLK_SKEW_HFP_DL, DSP_CLK_SKEW_MEMSIZE*2+DSP_CLK_SKEW_TEMP_BUF_SIZE,                          0,  stream_function_clock_skew_downlink_initialize, stream_function_clock_skew_hfp_downlink_process},/*0x2B FUNC_CLK_SKEW_DL,   */
#else
    {FUNC_CLK_SKEW_HFP_DL,                                                 0,                          0,                                            NULL,                                            NULL},/*0x2B FUNC_CLK_SKEW_DL,   */
#endif
    {FUNC_PROC_SIZE_CONV,                                                  0,                          0,       stream_function_size_converter_initialize,          stream_function_size_converter_process},/*0x2C FUNC_PROC_SIZE_CONV     */ /* Convert Processing size to give fixed given size*/
    {FUNC_JOINT,                                                           0,                          0,                                            NULL,                                            NULL},/*0x2D FUNC_JOINT,       */
    {FUNC_BRANCH,                                                          0,                          0,                                            NULL,                                            NULL},/*0x2E FUNC_BRANCH,       */
    {FUNC_MUTE,                                                            0,                          0,                                            NULL,                                            NULL},/*0x2F FUNC_MUTE,       */
#ifdef AIR_DRC_ENABLE
    {FUNC_DRC,                                                             0,                          0,            stream_function_drc_audio_initialize,               stream_function_drc_audio_process},/*0x30 FUNC_DRC,       */
#else
    {FUNC_DRC,                                                             0,                          0,                                            NULL,                                            NULL},/*0x30 FUNC_DRC,       */
#endif
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    {FUNC_PEQ,                                               DSP_PEQ_MEMSIZE,                          0,                  stream_function_peq_initialize,                     stream_function_peq_process},/*0x31 FUNC_PEQ,       */
#else
    {FUNC_PEQ,                                                             0,                          0,                                            NULL,                                            NULL},/*0x31 FUNC_PEQ,       */
#endif
    {FUNC_LPF,                                                             0,                          0,                                            NULL,                                            NULL},/*0x32 FUNC_LPF,       */

#if defined(MTK_BT_A2DP_ENABLE) || defined(AIR_BT_CODEC_BLE_ENABLED)
    {FUNC_CH_SEL,                                                          0,                          0,stream_function_channel_selector_initialize_a2dp,   stream_function_channel_selector_process_a2dp},/*0x33 FUNC_CH_SEL,       */
#else
    {FUNC_CH_SEL,                                                          0,                          0,                                            NULL,                                            NULL},/*0x33 FUNC_CH_SEL,       */
#endif
#ifdef AIR_MUTE_SMOOTHER_ENABLE
    {FUNC_MUTE_SMOOTHER,                                                   0,                          0,          stream_function_mute_smooth_initialize,             stream_function_mute_smooth_process},/*0x34 FUNC_MUTE_SMOOTHER,       */
#else
    {FUNC_MUTE_SMOOTHER,                                                   0,                          0,                                            NULL,                                            NULL},/*0x34 FUNC_MUTE_SMOOTHER,       */
#endif
#ifdef MTK_PEQ_ENABLE
    {FUNC_PEQ2,                                              DSP_PEQ_MEMSIZE,                          0,                 stream_function_peq2_initialize,                    stream_function_peq2_process},/*0x35 FUNC_PEQ2,     */
#else
    {FUNC_PEQ2,                                                            0,                          0,                                            NULL,                                            NULL},/*0x35 FUNC_PEQ2,     */
#endif
#ifdef AIR_DRC_ENABLE
    {FUNC_DRC2,                                                            0,                          0,           stream_function_drc_audio2_initialize,              stream_function_drc_audio2_process},/*0x36 FUNC_DRC2,       */
#else
    {FUNC_DRC2,                                                            0,                          0,                                            NULL,                                            NULL},/*0x36 FUNC_DRC2,       */
#endif
#ifdef MTK_BT_HFP_ENABLE
    {FUNC_CH_SEL_HFP,                                                      0,                          0, stream_function_channel_selector_initialize_hfp,    stream_function_channel_selector_process_hfp},/*0x37 FUNC_CH_SEL_HFP,       */
#else
    {FUNC_CH_SEL_HFP,                                                      0,                          0,                                            NULL,                                            NULL},/*0x37 FUNC_CH_SEL_HFP,       */
#endif
    {0,                                                                    0,                          0,                                            NULL,                                            NULL},/*0x39 _Reserved,       */
#ifdef MTK_VOICE_AGC_ENABLE
    {FUNC_RX_WB_AGC,                                         DSP_AGC_MEMSIZE,                          0,               stream_function_rx_agc_initialize,                  stream_function_rx_agc_process},/*0x3C FUNC_RX_WB_AGC,       */
    {FUNC_RX_NB_AGC,                                         DSP_AGC_MEMSIZE,                          0,               stream_function_rx_agc_initialize,                  stream_function_rx_agc_process},/*0x3D FUNC_RX_WB_AGC,       */
    {FUNC_TX_AGC,                                            DSP_AGC_MEMSIZE,                          0,               stream_function_tx_agc_initialize,                  stream_function_tx_agc_process},/*0x3E FUNC_RX_WB_AGC,       */
#else
    {FUNC_RX_WB_AGC,                                                       0,                          0,                                            NULL,                                            NULL},/*0x3C FUNC_RX_WB_AGC,       */
    {FUNC_RX_NB_AGC,                                                       0,                          0,                                            NULL,                                            NULL},/*0x3D FUNC_RX_WB_AGC,       */
    {FUNC_TX_AGC,                                                          0,                          0,                                            NULL,                                            NULL},/*0x3E FUNC_RX_WB_AGC,       */
#endif
#ifdef MTK_AUDIO_PLC_ENABLE
      {FUNC_AUDIO_PLC,                                     AUDIO_PLC_MEMSIZE,                          0,                                  Audio_PLC_Init,                               Audio_PLC_Process},/*0x40 FUNC_AUDIO_PLC,       */
#else
      {FUNC_AUDIO_PLC,                                                     0,                       0xFF,                                            NULL,                                            NULL},/*0x40 FUNC_AUDIO_PLC,       */
#endif
#ifdef MTK_PEQ_ENABLE
    {FUNC_PEQ_INSTANT,                                       DSP_PEQ_MEMSIZE,                          0,                  stream_function_peq_initialize,             stream_function_instant_peq_process},/*0x42 FUNC_PEQ_INSTANT,     */
#else
    {FUNC_PEQ_INSTANT,                                                     0,                          0,                                            NULL,                                            NULL},/*0x42 FUNC_PEQ_INSTANT,     */
#endif
#ifdef AIR_VOICE_NR_ENABLE
#ifdef MTK_3RD_PARTY_NR
    {FUNC_TX_EQ,                                                           0,                          0,                stream_function_tx_eq_initialize,                   stream_function_tx_eq_process},/*0x4C FUNC_TX_EQ,       */
#else
    {FUNC_TX_EQ,                                                           0,                          0,                                            NULL,                                            NULL},/*0x4C FUNC_TX_EQ,       */
#endif
#else
    {FUNC_TX_EQ,                                                           0,                          0,                                            NULL,                                            NULL},/*0x4C FUNC_TX_EQ,       */
#endif
#ifdef AIR_DRC_ENABLE
      {FUNC_DRC3,                                                          0,                          0,           stream_function_drc_audio3_initialize,              stream_function_drc_audio3_process},/*0x51 FUNC_DRC3,       */
#else
      {FUNC_DRC3,                                                          0,                          0,                                            NULL,                                            NULL},/*0x51 FUNC_DRC3,       */
#endif
#ifdef AIR_SOFTWARE_SRC_ENABLE
    {FUNC_SW_SRC,                                                          0,                          0,               stream_function_sw_src_initialize,                  stream_function_sw_src_process},/*0x52 FUNC_SW_SRC,       */
#else
    {FUNC_SW_SRC,                                                          0,                          0,                                            NULL,                                            NULL},/*0x52 FUNC_SW_SRC,       */
#endif
#ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    {FUNC_SW_CLK_SKEW,                                                     0,                          0,          stream_function_sw_clk_skew_initialize,             stream_function_sw_clk_skew_process},/*0x53 FUNC_SW_CLK_SKEW,       */
#else
    {FUNC_SW_CLK_SKEW,                                                     0,                          0,                                            NULL,                                            NULL},/*0x53 FUNC_SW_CLK_SKEW,       */
#endif
#ifdef AIR_SOFTWARE_GAIN_ENABLE
    {FUNC_SW_GAIN,                                                         0,                          0,              stream_function_sw_gain_initialize,                 stream_function_sw_gain_process},/*0x54 FUNC_SW_GAIN,       */
#else
    {FUNC_SW_GAIN,                                                         0,                          0,                                            NULL,                                            NULL},/*0x54 FUNC_SW_GAIN,       */
#endif
#ifdef AIR_SOFTWARE_MIXER_ENABLE
    {FUNC_SW_MIXER,                                                        0,                          0,             stream_function_sw_mixer_initialize,                stream_function_sw_mixer_process},/*0x55 FUNC_SW_MIXER,       */
#else
    {FUNC_SW_MIXER,                                                        0,                          0,                                            NULL,                                            NULL},/*0x55 FUNC_SW_MIXER,       */
#endif
#ifdef AIR_SOFTWARE_BUFFER_ENABLE
    {FUNC_SW_BUFFER,                                                       0,                          0,            stream_function_sw_buffer_initialize,               stream_function_sw_buffer_process},/*0x56 FUNC_SW_BUFFER,       */
#else
    {FUNC_SW_BUFFER,                                                       0,                          0,                                            NULL,                                            NULL},/*0x56 FUNC_SW_BUFFER,       */
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
    {FUNC_CLK_SKEW_BLE_MUSIC_DL, DSP_CLK_SKEW_MEMSIZE*2+DSP_CLK_SKEW_TEMP_BUF_SIZE,                    0,  stream_function_clock_skew_downlink_initialize, stream_function_clock_skew_ble_music_downlink_process},/*0x57 FUNC_CLK_SKEW_DL,   */
#else
    {FUNC_CLK_SKEW_BLE_MUSIC_DL,                                           0,                          0,                                            NULL,                                            NULL},/*0x57 FUNC_CLK_SKEW_DL,   */
#endif
#if 0 //def MTK_PROMPT_SOUND_ENABLE
    {FUNC_CH_SEL_VP,                                                       0,                          0,  stream_function_channel_selector_initialize_vp,    stream_function_channel_selector_process_vp},/*0x59 FUNC_CH_SEL_VP,	   */
#else
    {FUNC_CH_SEL_VP,                                                       0,                          0,                                            NULL,                                            NULL},/*0x59 FUNC_CH_SEL_VP,	   */
#endif
#ifdef AIR_AFC_ENABLE
    {FUNC_AFC,                                                             0,                          0,                  stream_function_afc_initialize,                     stream_function_afc_process},/*0x5B FUNC_AFC,           */
#else
    {FUNC_AFC,                                                             0,                          0,                                            NULL,                                            NULL},/*0x5B FUNC_AFC,           */
#endif
#ifdef AIR_LD_NR_ENABLE
    {FUNC_LD_NR,                                                           0,                          0,                stream_function_ld_nr_initialize,                   stream_function_ld_nr_process},/*0x5C FUNC_LD_NR,         */
#else
    {FUNC_LD_NR,                                                           0,                          0,                                            NULL,                                            NULL},/*0x5C FUNC_LD_NR,         */
#endif
#ifdef AIR_AT_AGC_ENABLE
    {FUNC_AT_AGC,                                                          0,                          0,               stream_function_at_agc_initialize,                  stream_function_at_agc_process},/*0x5D FUNC_AT_AGC,        */
#else
    {FUNC_AT_AGC,                                                          0,                          0,                                            NULL,                                            NULL},/*0x5D FUNC_AT_AGC,        */
#endif
#ifdef AIR_VOICE_NR_ENABLE
#if defined(AIR_EC120_ENABLE)
    {FUNC_EC120,                                              IGO_NR_MEMSIZE,                          0,               stream_function_ec_120_initialize,                   stream_function_ec120_process},/*0x60 FUNC_EC120,    */
#elif defined(AIR_EC120_ENABLE)
    {FUNC_EC120,                                                           0,                          0,               stream_function_ec_120_initialize,                   stream_function_ec120_process},/*0x60 FUNC_EC120,    */
#else
    {FUNC_EC120,                                                           0,                          0,                                            NULL,                                            NULL},/*0x60 FUNC_EC120,    */
#endif
#else
    {FUNC_DNN_NR,                                                          0,                          0,                                            NULL,                                            NULL},/*0x5F FUNC_DNN_NR,    */
    {FUNC_EC120,                                                           0,                          0,                                            NULL,                                            NULL},/*0x5F FUNC_DNN_NR,    */
#endif
#ifdef AIR_FIXED_RATIO_SRC
     {FUNC_SRC_FIXED_RATIO,                                                0,                          0,      stream_function_src_fixed_ratio_initialize,         stream_function_src_fixed_ratio_process},/*0x61 FUNC_SRC_FIXED_RATIO,     */
#else
     {FUNC_SRC_FIXED_RATIO,                                                0,                          0,                                            NULL,                                            NULL},/*0x61 FUNC_SRC_FIXED_RATIO,     */
#endif

 };
 /*==========================================================================================================================================================================================================*/


                                                         /*             feature_type,                         memory_size,     codec_output_size,                           initialize_entry,                      process_entry*/
stream_feature_codec_t     stream_feature_sample_codec    = {   CODEC_DECODER_SAMPLE,            CODEC_SAMPLE_MEMORY_SIZE,     CODEC_OUTPUT_SIZE,     stream_codec_decoder_sample_initialize, stream_codec_decoder_sample_process};

                                                          /*            feature_type,                         memory_size, 0(Must equal to zero),                           initialize_entry,                  process_entry*/
stream_feature_function_t  stream_feature_sample_function = {            FUNC_SAMPLE,         FUNCTION_SAMPLE_MEMORY_SIZE,                     0,          stream_function_sample_initialize, stream_function_sample_process};


stream_feature_ctrl_entry_t stream_feature_sample_entry = {stream_feature_sample_open, stream_feature_sample_close};

#ifdef CFG_CM4_PLAYBACK_ENABLE
stream_feature_list_t stream_feature_list_playback[] =
{
    CODEC_PCM_COPY,
    FUNC_END,
};
#endif

#ifdef MTK_BT_HFP_ENABLE
stream_feature_list_t stream_feature_list_hfp_uplink[] =
{
    CODEC_PCM_COPY,
    FUNC_CH_SEL_HFP,
#ifdef AIR_SOFTWARE_GAIN_ENABLE
    FUNC_SW_GAIN,
#else
    FUNC_MIC_SW_GAIN,
#endif
    FUNC_CLK_SKEW_UL,
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
    FUNC_SRC_FIXED_RATIO,
#endif
    FUNC_TX_NR,
#ifdef MTK_3RD_PARTY_NR
    FUNC_TX_EQ,
#endif
    FUNC_TX_WB_DRC,
#ifdef MTK_VOICE_AGC_ENABLE
    FUNC_TX_AGC,
#endif
    CODEC_ENCODER_MSBC,
    FUNC_END,
};

stream_feature_list_t stream_feature_list_hfp_downlink[] =
{
    CODEC_DECODER_MSBC,
    FUNC_PLC,
    FUNC_RX_NR,
    #ifdef MTK_VOICE_AGC_ENABLE
    FUNC_RX_WB_AGC,
    #endif
    FUNC_RX_WB_DRC,
    FUNC_CLK_SKEW_HFP_DL,
    FUNC_END,
};
#endif

#ifdef MTK_BT_A2DP_ENABLE
stream_feature_list_t stream_feature_list_a2dp[] =
{
    CODEC_DECODER_SBC,
#ifdef MTK_AUDIO_PLC_ENABLE
    FUNC_AUDIO_PLC,
#endif

    FUNC_CH_SEL,
#ifdef MTK_PEQ_ENABLE
    FUNC_PEQ,
    FUNC_PEQ2,
    FUNC_DRC,
#endif
/*
    FUNC_MUTE_SMOOTHER,
    //FUNC_PROC_SIZE_CONV,
*/
    FUNC_CLK_SKEW_DL,
    FUNC_END,

};
#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
stream_feature_list_t stream_feature_list_prompt[] =
{
    CODEC_PCM_COPY,
    //FUNC_CH_SEL_VP,
    FUNC_END,
};
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
stream_feature_list_t AudioFeatureList_BLE_Call_UL[] =
{
    CODEC_PCM_COPY,
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
    FUNC_SRC_FIXED_RATIO,
#endif
#ifdef AIR_SOFTWARE_GAIN_ENABLE
        FUNC_SW_GAIN,
#else
        FUNC_MIC_SW_GAIN,
#endif
    FUNC_CLK_SKEW_UL,
    FUNC_TX_NR,
#ifdef MTK_3RD_PARTY_NR
    FUNC_TX_EQ,
#endif
    FUNC_TX_WB_DRC,
#ifdef MTK_VOICE_AGC_ENABLE
    FUNC_TX_AGC,
#endif
    CODEC_ENCODER_LC3,
    FUNC_END,
};

stream_feature_list_t AudioFeatureList_BLE_Music_DL[] =
{
    CODEC_DECODER_LC3,
    FUNC_CH_SEL,
#ifdef MTK_PEQ_ENABLE
    FUNC_PEQ,
    FUNC_PEQ2,
    FUNC_DRC,
#endif
    FUNC_CLK_SKEW_BLE_MUSIC_DL,
    FUNC_END,
};

stream_feature_list_t AudioFeatureList_BLE_Call_DL[] =
{
    CODEC_DECODER_LC3,
    FUNC_RX_NR,
#ifdef MTK_VOICE_AGC_ENABLE
    FUNC_RX_WB_AGC,
#endif
    FUNC_RX_WB_DRC,
    FUNC_CLK_SKEW_HFP_DL,
    FUNC_END,
};
#endif



static const uint16_t coefficient_table_16k[13] = { //13-tap
    0x0127, 0x027A, 0x0278, 0x0227,
    0xFFD5, 0xFD22, 0xFABF, 0xFAEB,
    0xFE90, 0x05EB, 0x0F47, 0x180A,
    0x1D4E
};

////////////////////////////////////////////////////////////////////////////////
// DSPMEM FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * dsp_sdk_add_feature
 *
 * Add customer's feature to feature table
 *
 *
 *
 */
void dsp_sdk_initialize(void)
{
    dsp_sdk_add_feature_table(&stream_feature_sample_codec);
    dsp_sdk_add_feature_table(&stream_feature_sample_function);
    dsp_sdk_add_feature_ctrl(&stream_feature_sample_function, &stream_feature_sample_entry);
}

/**
 * stream_codec_decoder_sample_process
 *
 *
 */
bool stream_codec_decoder_sample_initialize(void *para)
{
    stream_codec_sample_instance_ptr_t codec_sample_pointer;

    //Get working buffer pointer
    codec_sample_pointer = (stream_codec_sample_instance_ptr_t)stream_codec_get_workingbuffer(para);

    //Do initialize
    codec_sample_pointer->memory_check = true;
    codec_sample_pointer->reset_check = false;
    memset(codec_sample_pointer->scratch_memory, 0xFF, 128);

    //return 0 when successfully initialize
    return 0;
}


/**
 * stream_codec_decoder_sample_process
 *
 *
 */
bool stream_codec_decoder_sample_process(void *para)
{
    stream_codec_sample_instance_ptr_t codec_sample_pointer;
    uint8_t *pattern_input_pointer;
    int16_t *output_l_pointer, *output_r_pointer;
    uint32_t input_length, output_length;

    //Get working buffer pointer, stream buffer pointer, and length
    codec_sample_pointer = (stream_codec_sample_instance_ptr_t)stream_codec_get_workingbuffer(para);
    pattern_input_pointer = stream_codec_get_input_buffer(para, 1);
    output_l_pointer = stream_codec_get_output_buffer(para, 1);
    output_r_pointer = stream_codec_get_output_buffer(para, 2);
    input_length = stream_codec_get_input_size(para);
    output_length = stream_codec_get_output_size(para);


    //Call decoder
    //output sample rate : 16kHz
    //output resolution  : 16-bit
    output_length = codec_decoder_sample_api(pattern_input_pointer, output_l_pointer, output_r_pointer, input_length);

    //Check decoder output
    if (output_length == 0) {
        //Do reinitialize when an error occurs.
        stream_feature_reinitialize(para);
    }

    //Check expected resolution
    if (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT) {
        dsp_converter_16bit_to_32bit((int32_t*)output_l_pointer, (int16_t*)output_l_pointer, output_length/sizeof(int16_t));
        dsp_converter_16bit_to_32bit((int32_t*)output_r_pointer, (int16_t*)output_r_pointer, output_length/sizeof(int16_t));
        output_length = output_length*2;
    }

    //Modify stream buffering format
    stream_codec_modify_output_samplingrate(para, FS_RATE_16K);
    stream_codec_modify_output_size(para, output_length);
    stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));

    //return 0 when successfully process
    return 0;
}

/**
 * stream_function_sample_initialize
 *
 *
 */
bool stream_function_sample_initialize(void *para)
{
    stream_function_sample_instance_ptr_t function_sample_pointer;

    //Get working buffer pointer
    function_sample_pointer = (stream_function_sample_instance_ptr_t)stream_function_get_working_buffer(para);

    //Do initialize
    memcpy(function_sample_pointer->filter, coefficient_table_16k, 13*sizeof(uint16_t));
    function_sample_pointer->coefficient_size = 13;

    //return 0 when successfully initialize
    return 0;
}

/**
 * stream_function_sample_process
 *
 *
 */
bool stream_function_sample_process(void *para)
{
    int16_t *l_pointer, *r_pointer;
    uint32_t frame_length;
    stream_function_sample_instance_ptr_t function_sample_pointer;

    //Get working buffer pointer, stream buffer pointer, and length
    function_sample_pointer = (stream_function_sample_instance_ptr_t)stream_function_get_working_buffer(para);
    l_pointer = stream_function_get_inout_buffer(para, 1);
    r_pointer = stream_function_get_inout_buffer(para, 2);
    frame_length = stream_function_get_output_size(para);

    //Call function API
    function_sample_api(function_sample_pointer,
                        l_pointer,
                        r_pointer,
                        function_sample_pointer->buffer_l,
                        function_sample_pointer->buffer_r,
                        frame_length);



    //return 0 when successfully process
    return 0;
}


void stream_feature_sample_open  (void)
{
    //dummy open entry
}


void stream_feature_sample_close (void)
{
    //dummy close enty
}


uint32_t codec_decoder_sample_api(uint8_t *pattern_pointer, int16_t *l_pointer, int16_t *r_pointer, uint32_t input_length)
{
    uint32_t codec_output_length = CODEC_OUTPUT_SIZE;
    UNUSED(pattern_pointer);
    UNUSED(l_pointer);
    UNUSED(r_pointer);
    UNUSED(input_length);
    return codec_output_length;
}

uint32_t function_sample_api(stream_function_sample_instance_ptr_t instance_ptr, int16_t *input_l_pointer, int16_t *input_r_pointer, int16_t *output_l_pointer, int16_t *output_r_pointer, uint32_t length)
{
    UNUSED(instance_ptr);
    UNUSED(input_l_pointer);
    UNUSED(input_r_pointer);
    UNUSED(output_l_pointer);
    UNUSED(output_r_pointer);
    UNUSED(length);
    return 0;
}

