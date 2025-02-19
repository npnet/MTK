include $(SOURCE_DIR)/$(WF_DRIVER_DIR)/config.mk


CFLAGS += -D_HIF_AXI=1
CFLAGS += -DMT7933
CFLAGS += -DCFG_SUPPORT_CONNAC2X=1
CFLAGS += -DCFG_MTK_ANDROID_WMT=0
CFLAGS += -DCFG_THIN_DRV
CFLAGS += -DCFG_REMOVE_NO_CALLER=1
CFLAGS += -DCFG_SUPPORT_LINK_QUALITY_MONITOR=0
CFLAGS += -DCFG_SUPPORT_QA_TOOL=0
CFLAGS += -DCFG_SUPPORT_ICAP=0
CFLAGS += -D__LITTLE_ENDIAN=1

CFLAGS += -DCFG_TC10_FEATURE=0
CFLAGS += -DCFG_TC1_FEATURE=0
CFLAGS += -DARP_MONITER_ENABLE=0
CFLAGS += -DCFG_SUPPORT_MANUAL_OWN_CTRL=1
CFLAGS += -DCFG_SUPPORT_PRIV_SUSPEND_MODE=1
CFLAGS += -DCFG_SUPPORT_PRIV_CHIP_CONFIG=1
CFLAGS += -DCFG_SUPPORT_PRIV_RUN_HQA=1
CFLAGS += -DCFG_SUPPORT_PRIV_GET_MIB_INFO=1
CFLAGS += -DCFG_SUPPORT_PRIV_SET_PM=1
CFLAGS += -DCFG_SUPPORT_PRIV_GET_DRV_VER=1
CFLAGS += -DCFG_SUPPORT_PRIV_FIXED_RATE=1
CFLAGS += -DCFG_SUPPORT_PRIV_STA_STAT=1
CFLAGS += -DCFG_SUPPORT_PRIV_WTBL_INFO=1
CFLAGS += -DCFG_SUPPORT_PRIV_SW_CTRL=1
CFLAGS += -DCFG_SUPPORT_PRIV_DBGLVL=1
CFLAGS += -DCFG_SUPPORT_PWR_LIMIT_COUNTRY=0
CFLAGS += -DCFG_SUPPORT_ARP_OFFLOAD_CMD=1
CFLAGS += -DCFG_WIFI_LP_IMPROVE=1

CFLAGS += -DCFG_SUPPORT_802_11W=1
CFLAGS += -DCFG_SUPPORT_WPA3=1

CFLAGS += -DCFG_SUPPORT_DEBUG_SOP=1

#/***** Manage configs into compile options ******/
ifneq ($(CFG_NUM_DIFFERENT_CHANNELS_STA),)
CFLAGS += -DCFG_NUM_DIFFERENT_CHANNELS_STA=$(CFG_NUM_DIFFERENT_CHANNELS_STA)
endif

ifneq ($(CFG_NUM_DIFFERENT_CHANNELS_P2P),)
CFLAGS += -DCFG_NUM_DIFFERENT_CHANNELS_P2P=$(CFG_NUM_DIFFERENT_CHANNELS_P2P)
endif

ifneq ($(CFG_DRIVER_INITIAL_RUNNING_MODE),)
CFLAGS += -DCFG_DRIVER_INITIAL_RUNNING_MODE=$(CFG_DRIVER_INITIAL_RUNNING_MODE)
endif

ifneq ($(CFG_ENABLE_WAKE_LOCK),)
CFLAGS += -DCFG_ENABLE_WAKE_LOCK=$(CFG_ENABLE_WAKE_LOCK)
endif

ifneq ($(CFG_DEFAULT_DBG_LEVEL),)
CFLAGS += -DCFG_DEFAULT_DBG_LEVEL=$(CFG_DEFAULT_DBG_LEVEL)
endif

ifneq ($(CFG_SUPPORT_ROAMING),)
CFLAGS += -DCFG_SUPPORT_ROAMING=$(CFG_SUPPORT_ROAMING)
endif

ifneq ($(CFG_SUPPORT_SAME_BSS_REASSOC),)
CFLAGS += -DCFG_SUPPORT_SAME_BSS_REASSOC=$(CFG_SUPPORT_SAME_BSS_REASSOC)
endif

ifneq ($(CFG_ENABLE_EFUSE_MAC_ADDR),)
CFLAGS += -DCFG_ENABLE_EFUSE_MAC_ADDR=$(CFG_ENABLE_EFUSE_MAC_ADDR)
endif

ifeq ($(CFG_DRIVER_INF_NAME_CHANGE), y)
CFLAGS += -DCFG_DRIVER_INF_NAME_CHANGE
endif

ifneq ($(CFG_SUPPORT_SINGLE_SKU_LOCAL_DB),)
CFLAGS += -DCFG_SUPPORT_SINGLE_SKU_LOCAL_DB=$(CFG_SUPPORT_SINGLE_SKU_LOCAL_DB)
endif

ifneq ($(CFG_SUPPORT_DFS_MASTER),)
CFLAGS += -DCFG_SUPPORT_DFS_MASTER=$(CFG_SUPPORT_DFS_MASTER)
endif

ifneq ($(CFG_P2P_SCAN_REPORT_ALL_BSS),)
CFLAGS += -DCFG_P2P_SCAN_REPORT_ALL_BSS=$(CFG_P2P_SCAN_REPORT_ALL_BSS)
endif

ifneq ($(CFG_SCAN_CHANNEL_SPECIFIED),)
CFLAGS += -DCFG_SCAN_CHANNEL_SPECIFIED=$(CFG_SCAN_CHANNEL_SPECIFIED)
endif

ifneq ($(CFG_CHIP_RESET_SUPPORT),)
CFLAGS += -DCFG_CHIP_RESET_SUPPORT=$(CFG_CHIP_RESET_SUPPORT)
endif

ifneq ($(CFG_RX_BA_ENTRY_MISS_TIMEOUT),)
CFLAGS += -DCFG_RX_BA_ENTRY_MISS_TIMEOUT=$(CFG_RX_BA_ENTRY_MISS_TIMEOUT)
endif

ifeq ($(CFG_SUPPORT_DISABLE_BCN_HINTS), y)
CFLAGS += -DCFG_SUPPORT_DISABLE_BCN_HINTS
endif

ifneq ($(CFG_SUPPORT_RSSI_COMP),)
CFLAGS += -DCFG_SUPPORT_RSSI_COMP=$(CFG_SUPPORT_RSSI_COMP)
endif

ifneq ($(CFG_EFUSE_AUTO_MODE_SUPPORT),)
CFLAGS += -DCFG_EFUSE_AUTO_MODE_SUPPORT=$(CFG_EFUSE_AUTO_MODE_SUPPORT)
endif

ifeq ($(CONFIG_IPV6), y)
CFLAGS += -DCONFIG_IPV6
endif

ifeq ($(CFG_SUPPORT_FREERTOS_NVRAM),y)
CFLAGS += -DCFG_SUPPORT_FREERTOS_NVRAM=1
else ifeq ($(CFG_SUPPORT_FREERTOS_NVRAM),n)
CFLAGS += -DCFG_SUPPORT_FREERTOS_NVRAM=0
endif

ifeq ($(CFG_SUPPORT_DIRECT_PBUF_RX), y)
CFLAGS += -DCFG_DIRECT_PBUF_RX
endif

ifeq ($(CFG_DIRECT_PBUF_WO_DMA), y)
CFLAGS += -DCFG_DIRECT_PBUF_WO_DMA=1
else
CFLAGS += -DCFG_DIRECT_PBUF_WO_DMA=0
endif

ifeq ($(CFG_SUPPORT_NO_SUPPLICANT_OPS), y)
CFLAGS += -DCFG_SUPPORT_NO_SUPPLICANT_OPS=1
CFLAGS += -DCFG_NO_SCANINFO_IN_DRV=0
else ifeq ($(CFG_SUPPORT_NO_SUPPLICANT_OPS), n)
CFLAGS += -DCFG_SUPPORT_NO_SUPPLICANT_OPS=0
ifeq ($(CFG_NO_SCANINFO_IN_DRV), y)
CFLAGS += -DCFG_NO_SCANINFO_IN_DRV=1
else ifeq ($(CFG_NO_SCANINFO_IN_DRV), n)
CFLAGS += -DCFG_NO_SCANINFO_IN_DRV=0
endif
endif

ifeq ($(CFG_SUPPORT_NO_SUPPLICANT_OPS_P2P), y)
CFLAGS += -DCFG_SUPPORT_NO_SUPPLICANT_OPS_P2P=1
else ifeq ($(CFG_SUPPORT_NO_SUPPLICANT_OPS_P2P), n)
CFLAGS += -DCFG_SUPPORT_NO_SUPPLICANT_OPS_P2P=0
endif

ifeq ($(CFG_SUPPORT_P2P), y)
CFLAGS += -DCFG_ENABLE_WIFI_DIRECT=1
CFLAGS += -DCFG_SUPPORT_DFS_MASTER=0
CFLAGS += -DCFG_SUPPORT_DFS=1
CFLAGS += -DCFG_SUPPORT_SAP_DFS_CHANNEL=0
CFLAGS += -DCFG_AUTO_CHANNEL_SEL_SUPPORT=0
CFLAGS += -DCFG_SUPPORT_WFD=0
CFLAGS += -DCFG_SUPPORT_WFD_COMPOSE_IE=0
CFLAGS += -DCFG_ENABLE_WIFI_DIRECT_CFG_80211=1
CFLAGS += -DCFG_DUAL_P2PLIKE_INTERFACE=0
CFLAGS += -DCFG_SUPPORT_P2P_GC=0
CFLAGS += -DCFG_SUPPORT_P2P_SCAN=0
CFLAGS += -DCFG_OPEN_GO_AS_AP_ONLY=0
CFLAGS += -DCFG_SUPPORT_HOTSPOT_WPS_MANAGER=0
else ifeq ($(CFG_SUPPORT_P2P), n)
CFLAGS += -DCFG_ENABLE_WIFI_DIRECT=0
CFLAGS += -DCFG_SUPPORT_DFS_MASTER=0
CFLAGS += -DCFG_SUPPORT_DFS=0
CFLAGS += -DCFG_AUTO_CHANNEL_SEL_SUPPORT=0
CFLAGS += -DCFG_SUPPORT_WFD=0
CFLAGS += -DCFG_SUPPORT_WFD_COMPOSE_IE=0
CFLAGS += -DCFG_SUPPORT_HOTSPOT_WPS_MANAGER=0
CFLAGS += -DCFG_ENABLE_WIFI_DIRECT_CFG_80211=0
CFLAGS += -DCFG_DUAL_P2PLIKE_INTERFACE=0
CFLAGS += -DCFG_SUPPORT_P2P_GC=0
CFLAGS += -DCFG_SUPPORT_P2P_SCAN=0
CFLAGS += -DCFG_OPEN_GO_AS_AP_ONLY=0
endif

ifeq ($(CFG_PROFILE_PBUF), y)
CFLAGS += -DCFG_PROFILE_PBUF=1
else
CFLAGS += -DCFG_PROFILE_PBUF=0
endif
# ---------------------------------------------------
# Compile Options
# ---------------------------------------------------

CFLAGS += -DCONFIG_WIFI_SUPPORT_MODULATION=$(CONFIG_WIFI_SUPPORT_MODULATION)
CFLAGS += -DCFG_SUPPORT_DEBUG_FS=0

ifneq ($(CFG_SUPPORT_TSF_USING_BOOTTIME), )
CFLAGS += -DCFG_SUPPORT_TSF_USING_BOOTTIME=$(CFG_SUPPORT_TSF_USING_BOOTTIME)
endif
#CFLAGS += -Werror //KOKO rtos remove from build warning as error
# for build time debug log level
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-unused-variable
CFLAGS:=$(filter-out -U$(WLAN_CHIP_ID),$(CFLAGS))
CFLAGS += -D$(WLAN_CHIP_ID)

ifeq ($(CONFIG_MTK_WIFI_MCC_SUPPORT), y)
    CFLAGS += -DCFG_SUPPORT_CHNL_CONFLICT_REVISE=0
else
    CFLAGS += -DCFG_SUPPORT_CHNL_CONFLICT_REVISE=1
endif

CFLAGS += -DCFG_SUPPORT_AEE=0


# Enable Metrics for Wlan reset
CFLAGS += -DCFG_AMAZON_METRICS_LOG=0

CFLAGS += -DBUILD_QA_DBG=0

CFLAGS += -DCFG_BUILT_IN_DRIVER=0

CFLAGS += -DCFG_SUPPORT_PWR_LIMIT_FILE_LOAD=0

#support STA + P2P GO MCC case
ifeq ($(CFG_SUPPORT_STA_P2P_MCC),y)
CFLAGS += -DSTA_P2P_MCC
endif

ifeq ($(CFG_SUPPORT_P2P_OPEN_SECURITY),y)
CFLAGS += -DCFG_SUPPORT_P2P_OPEN_SECURITY
endif

ifeq ($(CFG_SUPPORT_P2P_GO_11b_RATE),y)
CFLAGS += -DCFG_SUPPORT_P2P_GO_11b_RATE
endif

ifeq ($(CFG_SUPPORT_P2P_GO_KEEP_RATE_SETTING),y)
CFLAGS += -DCFG_SUPPORT_P2P_GO_KEEP_RATE_SETTING
endif

ifeq ($(CFG_SUPPORT_ADJUST_MCC_STAY_TIME),y)
CFLAGS += -DCFG_SUPPORT_ADJUST_MCC_STAY_TIME
endif

ifeq ($(CFG_ENABLE_DEWEIGHTING_CTRL), y)
    CFLAGS += -DCFG_ENABLE_DEWEIGHTING_CTRL=1
else
    CFLAGS += -DCFG_ENABLE_DEWEIGHTING_CTRL=0
endif

#For adjust channel request interval when ais join net work
ifeq ($(CFG_SUPPORT_ADJUST_JOIN_CH_REQ_INTERVAL),y)
CFLAGS += -DCFG_SUPPORT_ADJUST_JOIN_CH_REQ_INTERVAL
endif

ifeq ($(CFG_IPI_2CHAIN_SUPPORT),n)
CFLAGS += -DCFG_IPI_2CHAIN_SUPPORT=0
else
CFLAGS += -DCFG_IPI_2CHAIN_SUPPORT=1
endif

ifeq ($(CFG_ENABLE_PS_INTV_CTRL), y)
    CFLAGS += -DCFG_ENABLE_PS_INTV_CTRL=1
else
    CFLAGS += -DCFG_ENABLE_PS_INTV_CTRL=0
endif

ifeq ($(CFG_ENABLE_1RPD_MMPS_CTRL), y)
    CFLAGS += -DCFG_ENABLE_1RPD_MMPS_CTRL=1
else
    CFLAGS += -DCFG_ENABLE_1RPD_MMPS_CTRL=0
endif

ifeq ($(CONFIG_MTK_WIFI_TWT_SUPPORT), y)
    ifeq ($(CONFIG_MTK_WIFI_11AX_SUPPORT), y)
        CFLAGS += -DCFG_SUPPORT_TWT=1
        CFLAGS += -DCFG_SUPPORT_802_11AX=1
    endif
endif

CFLAGS += -DCFG_WOW_SUPPORT=1

CFLAGS += -DCFG_SUPPORT_PASSPOINT=0
CFLAGS += -DCFG_HS20_DEBUG=0
CFLAGS += -DCFG_ENABLE_GTK_FRAME_FILTER=0

CFLAGS += -DCFG_MET_PACKET_TRACE_SUPPORT=0

CFLAGS += -DCFG_MET_TAG_SUPPORT=0

CFLAGS += -DDBG=0

CFLAGS += -DCFG_SUPPORT_PRIV_PROCESS_IT=0
CFLAGS += -DCFG_ENABLE_UNUSED_MSG=0

ifeq ($(MTK_NVDM_ENABLE), y)
CFLAGS += -DCFG_SUPPORT_NVDM=1
endif

ifneq ($(PLATFORM_FLAGS), )
    CFLAGS += $(PLATFORM_FLAGS)
endif

#
# mtprealloc
#
ifeq ($(CFG_MTK_PREALLOC_DRIVER), y)
CFLAGS += -DCFG_PREALLOC_MEMORY
endif

#
# wlan_service
#
CFLAGS += -DCONFIG_WLAN_SERVICE=$(CONFIG_WLAN_SERVICE)
CFLAGS += -DCONFIG_TEST_ENGINE_OFFLOAD=$(CONFIG_TEST_ENGINE_OFFLOAD)

#
# wifi test tool
#
CFLAGS += -DCONFIG_WIFI_TEST_TOOL=$(CONFIG_WIFI_TEST_TOOL)

#
# buffer bin from flash
#
CFLAGS += -DCFG_BUFFER_BIN_FROM_FLASH=$(CFG_BUFFER_BIN_FROM_FLASH)

#
# for switching the ram mode
#
CFLAGS += -DCFG_ZIDATA_IN_MEM=$(CFG_ZIDATA_IN_MEM)
CFLAGS += -DCFG_CODE_IN_MEM_TX=$(CFG_CODE_IN_MEM_TX)
CFLAGS += -DCFG_CODE_IN_MEM_RX=$(CFG_CODE_IN_MEM_RX)

#
# mshrink build mode
#
ifneq ($(MTK_RELEASE_MODE),)
CFLAGS += -DMTK_M_RELEASE=1
CFLAGS += -DMTK_M_DEBUG=2
CFLAGS += -DMTK_M_MFG=3
ifeq ($(MTK_RELEASE_MODE), release)
CFLAGS += -DMTK_RELEASE_MODE=MTK_M_RELEASE
endif
ifeq ($(MTK_RELEASE_MODE), debug)
CFLAGS += -DMTK_RELEASE_MODE=MTK_M_DEBUG
endif
ifeq ($(MTK_RELEASE_MODE),mfg)
CFLAGS += -DMTK_RELEASE_MODE=MTK_M_MFG
endif
endif

#
# buffer bin from flash
#
ifeq ($(CFG_P2P_DEV_FSM), y)
CFLAGS += -DCFG_P2P_DEV_FSM=1
else
CFLAGS += -DCFG_P2P_DEV_FSM=0
endif

# CSI PSRAM mode
ifeq ($(MTK_XMODEM_ENABLE), y)
ifeq ($(MTK_CSI_PSRAM_ENABLE), y)
CFLAGS += -DMTK_CSI_PSRAM_ENABLE
endif
endif
