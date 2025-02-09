
ifeq ($(MTK_HAL_ADC_MODULE_ENABLE), y)
CFLAGS   += -DHAL_ADC_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_GCPU_MODULE_ENABLE), y)
ifeq ($(MTK_HAL_AES_MODULE_ENABLE), y)
CFLAGS   += -DHAL_AES_MODULE_ENABLED
endif
endif

ifeq ($(MTK_HAL_CACHE_MODULE_ENABLE), y)
CFLAGS   += -DHAL_CACHE_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_CLOCK_MODULE_ENABLE), y)
CFLAGS   += -DHAL_CLOCK_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_GCPU_MODULE_ENABLE), y)
ifeq ($(MTK_HAL_DES_MODULE_ENABLE), y)
CFLAGS   += -DHAL_DES_MODULE_ENABLED
endif
endif

ifeq ($(MTK_HAL_EINT_MODULE_ENABLE), y)
CFLAGS   += -DHAL_EINT_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_FLASH_MODULE_ENABLE), y)
CFLAGS   += -DHAL_FLASH_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_GDMA_MODULE_ENABLE), y)
CFLAGS   += -DHAL_GDMA_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_GPIO_MODULE_ENABLE), y)
CFLAGS   += -DHAL_GPIO_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_GPT_MODULE_ENABLE), y)
CFLAGS   += -DHAL_GPT_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_SEJ_GPT_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SEJ_GPT_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_GCPU_MODULE_ENABLE), y)
CFLAGS   += -DHAL_GCPU_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_I2C_MASTER_MODULE_ENABLE), y)
CFLAGS   += -DHAL_I2C_MASTER_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_NVIC_MODULE_ENABLE), y)
CFLAGS   += -DHAL_NVIC_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_IRRX_MODULE_ENABLE), y)
CFLAGS   += -DHAL_IRRX_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_KEYPAD_MODULE_ENABLE), y)
CFLAGS   += -DHAL_KEYPAD_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_GCPU_MODULE_ENABLE), y)
ifeq ($(MTK_HAL_MD5_MODULE_ENABLE), y)
CFLAGS   += -DHAL_MD5_MODULE_ENABLED
endif
endif

ifeq ($(MTK_HAL_MPU_MODULE_ENABLE), y)
CFLAGS   += -DHAL_MPU_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_SECURITY_MODULE_ENABLE), y)
ifeq ($(MTK_HAL_ASIC_MPU_MODULE_ENABLE), y)
CFLAGS   += -DHAL_ASIC_MPU_MODULE_ENABLED
endif
ifeq ($(MTK_HAL_DEVAPC_MODULE_ENABLE), y)
CFLAGS   += -DHAL_DEVAPC_MODULE_ENABLED
endif
endif

ifeq ($(MTK_HAL_PMU_MODULE_ENABLE), y)
CFLAGS   += -DHAL_PMU_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_PMU_FORCE_VCORE_0P8V), y)
CFLAGS   += -DHAL_PMU_FORCE_VCORE_0P8V
endif

ifeq ($(MTK_HAL_CLK_FORCE_RTC_XOSC), y)
CFLAGS   += -DHAL_CLK_FORCE_RTC_XOSC
endif

ifeq ($(MTK_HAL_PSRAM_MODULE_ENABLE), y)
CFLAGS   += -DHAL_PSRAM_MODULE_ENABLED
CFLAGS   += -I$(SOURCE_DIR)/$(MT7933_CHIP)/src/psramc/inc
endif

ifeq ($(MTK_HAL_PSRAM_UHS_ENABLE), y)
CFLAGS   += -DHAL_PSRAM_UHS_ENABLED
else
CFLAGS   += -DHAL_PSRAM_NON_UHS_ENABLED
endif

ifeq ($(MTK_HAL_PWM_MODULE_ENABLE), y)
CFLAGS   += -DHAL_PWM_MODULE_ENABLED
CFLAGS   += -I$(SOURCE_DIR)/$(MT7933_CHIP)/src/pwm/inc
endif

ifeq ($(MTK_HAL_RTC_MODULE_ENABLE), y)
CFLAGS   += -DHAL_RTC_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_SPI_MASTER_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SPI_MASTER_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_SPI_SLAVE_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SPI_SLAVE_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_SD_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SD_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_SDIO_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SDIO_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_SDIO_SLAVE_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SDIO_SLAVE_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_GCPU_MODULE_ENABLE), y)
ifeq ($(MTK_HAL_SHA_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SHA_MODULE_ENABLED
endif
endif

ifeq ($(MTK_HAL_SLEEP_MANAGER_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SLEEP_MANAGER_ENABLED
endif

ifeq ($(MTK_HAL_TRNG_MODULE_ENABLE), y)
CFLAGS   += -DHAL_TRNG_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_UART_MODULE_ENABLE), y)
CFLAGS   += -DHAL_UART_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_USB_MODULE_ENABLE), y)
CFLAGS   += -DHAL_USB_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_WDT_MODULE_ENABLE), y)
CFLAGS   += -DHAL_WDT_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_DWT_MODULE_ENABLE), y)
CFLAGS   += -DHAL_DWT_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_CLK_CTP_SUPPORT), y)
CFLAGS   += -DHAL_CLK_CTP_SUPPORT
endif

ifeq ($(MTK_HAL_SPM_LOW_POWER_SUPPORT), y)
CFLAGS   += -DHAL_SPM_LOW_POWER_SUPPORT
endif

ifeq ($(MTK_HAL_TOP_THERMAL_MODULE_ENABLE), y)
CFLAGS   += -DHAL_TOP_THERMAL_MODULE_ENABLED
endif

ifeq ($(MTK_HAL_EFUSE_MODULE_ENABLE), y)
CFLAGS   += -I$(SOURCE_DIR)/$(MT7933_CHIP)/src/efuse/inc
endif

ifeq ($(MTK_HAL_KPD_MODULE_ENABLE), y)
CFLAGS   += -I$(SOURCE_DIR)/$(MT7933_CHIP)/src/keypad/inc
CFLAGS   += -I$(SOURCE_DIR)/$(MT7933_CHIP)/src/inc
endif

ifeq ($(MTK_HAL_SECURITY_MODULE_ENABLE), y)
CFLAGS   += -DHAL_SECURITY_MODULE_ENABLE
endif
#################################################################################

