# Copyright Statement:
#
# (C) 2005-2016  MediaTek Inc. All rights reserved.
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
# Without the prior written permission of MediaTek and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
# You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
# if you have agreed to and been bound by the applicable license agreement with
# MediaTek ("License Agreement") and been granted explicit permission to do so within
# the License Agreement ("Permitted User").  If you are not a Permitted User,
# please cease any access or use of MediaTek Software immediately.
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#

###################################################
# Sources
KERNEL_SERVICE_SRC    = kernel/service/src

KERNEL_SERVICE_FILES += $(KERNEL_SERVICE_SRC)/context_info_save.c \
                        $(KERNEL_SERVICE_SRC)/mtk_HeapSizeGuard.c \
                        $(KERNEL_SERVICE_SRC)/os_cli.c \
                        $(KERNEL_SERVICE_SRC)/os_port_callback.c \
                        $(KERNEL_SERVICE_SRC)/os_trace_callback.c \
                        $(KERNEL_SERVICE_SRC)/syslog.c \
                        $(KERNEL_SERVICE_SRC)/syslog_cli.c \
                        $(KERNEL_SERVICE_SRC)/toi.c \
                        $(KERNEL_SERVICE_SRC)/utils.c \
                        $(KERNEL_SERVICE_SRC)/swla.c

ifeq ($(MTK_LIGHT_WEIGHT_PRINTF_ENABLED), y)
KERNEL_SERVICE_FILES += $(KERNEL_SERVICE_SRC)/stdio_wrapper.c
endif

ifeq ($(MTK_MEMORY_EXTRACTOR_ENABLE), y)
KERNEL_SERVICE_FILES += $(KERNEL_SERVICE_SRC)/memextract.c
endif

ifeq ($(MTK_HAL_SLEEP_MANAGER_MODULE_ENABLE), y)
KERNEL_SERVICE_FILES += $(KERNEL_SERVICE_SRC)/os_sleep_callback.c
endif

ifeq ($(PRODUCT_VERSION),7933)
KERNEL_SERVICE_FILES += $(KERNEL_SERVICE_SRC)/os_utils.c
endif

# TODO: Remove this after creating project specific memory_regions.c for 2625
ifeq ($(PRODUCT_VERSION),2625)
KERNEL_SERVICE_FILES += $(KERNEL_SERVICE_SRC)/memory_regions.c
endif
ifneq ($(PRODUCT_VERSION),8512)
KERNEL_SERVICE_FILES += $(KERNEL_SERVICE_SRC)/exception_handler.c
endif

include $(SOURCE_DIR)/kernel/service/system_daemon/module.mk

C_FILES += $(KERNEL_SERVICE_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc


###################################################
# feature
ifeq ($(MTK_HAL_FLASH_MODULE_ENABLE), y)
ifeq ($(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE), y)
CFLAGS   += -DMTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE
endif
ifeq ($(MTK_MEMORY_EXTRACTOR_ENABLE), y)
CFLAGS   += -DMTK_MEMORY_EXTRACTOR_ENABLE
endif
endif

ifeq ($(MTK_OS_TIMER_LIST_ENABLE), y)
CFLAGS   += -DMTK_OS_TIMER_LIST_ENABLE
endif

ifeq ($(MTK_OS_SEMAPHORE_LIST_ENABLE), y)
CFLAGS   += -DMTK_OS_SEMAPHORE_LIST_ENABLE
endif
