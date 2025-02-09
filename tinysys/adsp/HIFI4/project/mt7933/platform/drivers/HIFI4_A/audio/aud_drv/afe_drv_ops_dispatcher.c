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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#include "afe_drv_ops_implement.h"
#include "afe_drv_ops_dispatcher.h"
#include "afe_drv_pcm.h"
#include "audio_rtos_header_group.h"
#include "semphr.h"
#include "audio_drv_log.h"

/* Just implement dummy, because most setting is implemented in Linux ALSA Drvier */
int do_pre_start_with_path_data(struct dsp_path_data *path_data)
{
    return 0;
}

int do_start_with_path_data(struct dsp_path_data *path_data)
{
    int i;

    for (i = 0; i < path_data->fe_num; ++i) {
        configASSERT(path_data->fe_ops[i] && path_data->fe_ops[i]->get_hw_mem_ptr);
        if (path_data->fe_ops[i] && path_data->fe_ops[i]->start)
            return path_data->fe_ops[i]->start(path_data);
    }
    return 0;
}

int do_stop_with_path_data(struct dsp_path_data *path_data)
{
    int i;

    for (i = 0; i < path_data->fe_num; ++i) {
        configASSERT(path_data->fe_ops[i] && path_data->fe_ops[i]->get_hw_mem_ptr);
        if (path_data->fe_ops[i] && path_data->fe_ops[i]->stop)
            return path_data->fe_ops[i]->stop(path_data);
    }
    return 0;
}

uint32_t get_hw_ptr_bias_with_path_data(struct dsp_path_data *path_data)
{
    int i;

    for (i = 0; i < path_data->fe_num; ++i) {
        configASSERT(path_data->fe_ops[i] && path_data->fe_ops[i]->get_hw_mem_ptr);
        if (path_data->fe_ops[i] && path_data->fe_ops[i]->get_hw_mem_ptr)
            return path_data->fe_ops[i]->get_hw_mem_ptr(path_data);
    }
    return 0;
}

uint32_t get_hw_mem_start_with_path_data(struct dsp_path_data *path_data)
{
    int i;

    for (i = 0; i < path_data->fe_num; ++i) {
        configASSERT(path_data->fe_ops[i] && path_data->fe_ops[i]->get_hw_mem_start);
        if (path_data->fe_ops[i] && path_data->fe_ops[i]->get_hw_mem_start)
            return path_data->fe_ops[i]->get_hw_mem_start(path_data);
    }
    return 0;
}

uint32_t get_hw_mem_cur_with_path_data(struct dsp_path_data *path_data)
{
    int i;

    for (i = 0; i < path_data->fe_num; ++i) {
        configASSERT(path_data->fe_ops[i] && path_data->fe_ops[i]->get_hw_mem_cur);
        if (path_data->fe_ops[i] && path_data->fe_ops[i]->get_hw_mem_cur)
            return path_data->fe_ops[i]->get_hw_mem_cur(path_data);
    }
    return 0;
}

