/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
/*! \file   "hif.h"
 *  \brief  Functions for the driver to register bus and setup the IRQ
 *
 *  Functions for the driver to register bus and setup the IRQ
 */

#ifndef _HIF_H
#define _HIF_H

#define LINUX_USE	0
#define FREERTOS_USE	1

#include "typedef.h"
#include "gl_os.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"
#include "connsys_wifi_queue.h"
#include <timers.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "wifi_freertos_adapter.h"

#include "hif_pdma.h"


#if defined(_HIF_AXI)
#define HIF_NAME "AXI"
#else
#error "No HIF defined!"
#endif

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */
extern phys_addr_t gWifiRsvMemPhyBase;
extern unsigned long long gWifiRsvMemSize;
#if (CFG_SUPPORT_CONNINFRA == 1)
//extern wait_queue_head_t g_waitq_rst;
extern unsigned long g_ulFlag;
extern KAL_WAKE_LOCK_T g_IntrWakeLock;
#endif

#if CFG_ENABLE_WFDMA_DVT
bool halWfdmaDvtInit(IN struct ADAPTER *prAdapter, IN uint8_t ucMode);

struct wfdma_dvt {
	uint8_t  enable;
	uint8_t  wifi_off;
	uint8_t  err;
	uint8_t  rx_mode;
	uint8_t  tx_start;
	uint8_t  rx_start;
	uint32_t tx_counts[WFDMA_DVT_RING_NUM];
	uint32_t rx_counts[WFDMA_DVT_RING_NUM];
};
#endif
/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */
#define AXI_CFG_PREALLOC_MEMORY_BUFFER    1
#define AXI_CFG_HOST_PRESERVED 0

#define AXI_TX_MAX_SIZE_PER_FRAME         (NIC_TX_MAX_SIZE_PER_FRAME +      \
					   NIC_TX_DESC_AND_PADDING_LENGTH)

#define AXI_TX_CMD_BUFF_SIZE              4096
#define AXI_WLAN_IRQ_NUMBER	CONN2AP_WFDMA_IRQn
#define AXI_WF2AP_SW_IRQ_NUMBER WF2AP_SW_IRQn

#define request_irq(_irq_num, _irq_handler, _irq_type, _dev_name, _glueinfo) \
({ \
	int _res = 0; \
	_res = hal_nvic_register_isr_handler(_irq_num, _irq_handler); \
	_res = hal_nvic_irq_set_type(_irq_num, _irq_type); \
	_res; \
})
/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

#if defined(_HIF_AXI) && (CFG_SUPPORT_MANUAL_OWN_CTRL == 1)
enum axi_state {
	AXI_STATE_REDAY,
	AXI_STATE_FORCE_FW_OWN
};
#endif /* defined(_HIF_AXI) && (CFG_SUPPORT_MANUAL_OWN_CTRL == 1) */

struct connsys_wf_hif_op {
	int (*probe)(void);
	int (*remove)(void);
	void (*shutdown)(void);
	int (*suspend)(void *Data);
	int (*resume)(void *Data);
	struct mt66xx_hif_driver_data *driver_data;
};

struct GL_HIF_INFO;

struct HIF_MEM_OPS {
	void (*allocTxDesc)(struct GL_HIF_INFO *prHifInfo,
			    struct RTMP_DMABUF *prDescRing,
			    uint32_t u4Num);
	void (*allocRxDesc)(struct GL_HIF_INFO *prHifInfo,
			    struct RTMP_DMABUF *prDescRing,
			    uint32_t u4Num);
	void (*allocTxCmdBuf)(struct RTMP_DMABUF *prDmaBuf,
			      uint32_t u4Num, uint32_t u4Idx);
	void (*allocTxDataBuf)(struct MSDU_TOKEN_ENTRY *prToken,
			       uint32_t u4Idx);
	void *(*allocRxBuf)(struct GL_HIF_INFO *prHifInfo,
			    struct RTMP_DMABUF *prDmaBuf,
			    uint32_t u4Num, uint32_t u4Idx);
	void *(*allocRuntimeMem)(uint32_t u4SrcLen);
	bool (*copyCmd)(struct GL_HIF_INFO *prHifInfo,
			struct RTMP_DMACB *prTxCell, void *pucBuf,
			void *pucSrc1, uint32_t u4SrcLen1,
			void *pucSrc2, uint32_t u4SrcLen2);
	bool (*copyEvent)(struct GL_HIF_INFO *prHifInfo,
			  struct RTMP_DMACB *pRxCell,
			  struct RXD_STRUCT *pRxD,
			  struct RTMP_DMABUF *prDmaBuf,
			  uint8_t *pucDst, uint32_t u4Len);
	bool (*copyTxData)(struct MSDU_TOKEN_ENTRY *prToken,
			   void *pucSrc, uint32_t u4Len);
	bool (*copyRxData)(struct GL_HIF_INFO *prHifInfo,
			   struct RTMP_DMACB *pRxCell,
			   struct RTMP_DMABUF *prDmaBuf,
			   struct SW_RFB *prSwRfb);
	void (*flushCache)(struct GL_HIF_INFO *prHifInfo,
			   void *pucSrc, uint32_t u4Len);
	phys_addr_t (*mapTxBuf)(struct GL_HIF_INFO *prHifInfo,
			  void *pucBuf, uint32_t u4Offset, uint32_t u4Len);
	phys_addr_t (*mapRxBuf)(struct GL_HIF_INFO *prHifInfo,
			  void *pucBuf, uint32_t u4Offset, uint32_t u4Len);
	void (*unmapTxBuf)(struct GL_HIF_INFO *prHifInfo,
			   phys_addr_t rDmaAddr, uint32_t u4Len);
	void (*unmapRxBuf)(struct GL_HIF_INFO *prHifInfo,
			   phys_addr_t rDmaAddr, uint32_t u4Len);
	void (*freeDesc)(struct GL_HIF_INFO *prHifInfo,
			 struct RTMP_DMABUF *prDescRing);
	void (*freeBuf)(void *pucSrc, uint32_t u4Len);
	void (*freePacket)(void *pvPacket);
	void (*dumpTx)(struct GL_HIF_INFO *prHifInfo,
		       struct RTMP_TX_RING *prTxRing,
		       uint32_t u4Idx, uint32_t u4DumpLen);
	void (*dumpRx)(struct GL_HIF_INFO *prHifInfo,
		       struct RTMP_RX_RING *prRxRing,
		       uint32_t u4Idx, uint32_t u4DumpLen);
};

/* host interface's private data structure, which is attached to os glue
 ** layer info structure.
 */
struct GL_HIF_INFO {
	//struct platform_device *pdev;
	//struct device *prDmaDev;
	struct HIF_MEM_OPS rMemOps;

	uint32_t u4IrqId;
	uint32_t u4IrqId_1;
	int32_t u4HifCnt;

	/* AXI MMIO Base Address, all access will use */
	void *CSRBaseAddress;

	/* Shared memory of all 1st pre-allocated
	 * TxBuf associated with each TXD
	 */
	/* Shared memory for Tx descriptors */
	struct RTMP_DMABUF TxDescRing[NUM_OF_TX_RING];
	struct RTMP_TX_RING TxRing[NUM_OF_TX_RING];	/* AC0~3 + HCCA */

	/* Shared memory for RX descriptors */
	struct RTMP_DMABUF RxDescRing[NUM_OF_RX_RING];
	struct RTMP_RX_RING RxRing[NUM_OF_RX_RING];

	uint8_t fgIntReadClear;
	uint8_t fgMbxReadClear;

	uint32_t u4IntStatus;

	struct MSDU_TOKEN_INFO rTokenInfo;

	struct ERR_RECOVERY_CTRL_T rErrRecoveryCtl;
	uint8_t fgIsErrRecovery;
	//struct timer_list rSerTimer;
	struct LINK rTxCmdQ;
	struct LINK rTxDataQ;
	uint32_t u4TxDataQLen;

	bool fgIsPowerOff;
	bool fgIsDumpLog;

#if defined(_HIF_AXI) && (CFG_SUPPORT_MANUAL_OWN_CTRL == 1)
	enum axi_state state;
#endif /* defined(_HIF_AXI) && (CFG_SUPPORT_MANUAL_OWN_CTRL == 1) */
};

struct BUS_INFO {
	const uint32_t top_cfg_base;	/* TOP_CFG_BASE address */
	const struct PCIE_CHIP_CR_MAPPING *bus2chip;
	const uint32_t tx_ring_cmd_idx;
	const uint32_t tx_ring_wa_cmd_idx;
	const uint32_t tx_ring_fwdl_idx;
	const uint32_t tx_ring0_data_idx;
	const uint32_t tx_ring1_data_idx;
	const uint32_t max_static_map_addr;
	const uint32_t fw_own_clear_addr;
	const uint32_t fw_own_clear_bit;
	const bool fgCheckDriverOwnInt;
	const uint32_t u4DmaMask;
	/* host pdma/wfdma0 base address */
	const uint32_t host_dma0_base;
	/* host wfdma1 base address */
	const uint32_t host_dma1_base;
	/* host ext conn hif wrap */
	const uint32_t host_ext_conn_hif_wrap_base;
	const uint32_t host_int_status_addr;
	const uint32_t host_int_txdone_bits;
	const uint32_t host_int_rxdone_bits;

	/* tx pdma/wfdma ring base address */
	const uint32_t host_tx_ring_base;
	/* tx pdma/wfdma ring ext control base address */
	const uint32_t host_tx_ring_ext_ctrl_base;
	/* tx pdma/wfdma ring cpu index address */
	const uint32_t host_tx_ring_cidx_addr;
	/* tx pdma/wfdma ring dma index address */
	const uint32_t host_tx_ring_didx_addr;
	/* tx pdma/wfdma ring count address */
	const uint32_t host_tx_ring_cnt_addr;

	/* rx pdma/wfdma ring base address */
	const uint32_t host_rx_ring_base;
	/* rx pdma/wfdma ring ext control base address */
	const uint32_t host_rx_ring_ext_ctrl_base;
	/* rx pdma/wfdma ring cpu index address */
	const uint32_t host_rx_ring_cidx_addr;
	/* rx pdma/wfdma ring dma index address */
	const uint32_t host_rx_ring_didx_addr;
	/* rx pdma/wfdma ring count address */
	const uint32_t host_rx_ring_cnt_addr;
	/*skip tx ring index*/
	const uint32_t skip_tx_ring;

#if (CFG_SUPPORT_CONNAC2X == 1)
	/* rx wfdma_1 ring base address */
	const uint32_t host_wfdma1_rx_ring_base;
	/* rx wfdma_1 ring cpu index address */
	const uint32_t host_wfdma1_rx_ring_cidx_addr;
	/* rx wfdma_1 ring dma index address */
	const uint32_t host_wfdma1_rx_ring_didx_addr;
	/* rx wfdma_1 ring count address */
	const uint32_t host_wfdma1_rx_ring_cnt_addr;
	/* rx wfdma_1 ring ext control base address */
	const uint32_t host_wfdma1_rx_ring_ext_ctrl_base;
#endif /* CFG_SUPPORT_CONNAC2X == 1 */

	void (*pdmaSetup)(struct GLUE_INFO *prGlueInfo, uint8_t enable);
	uint32_t (*updateTxRingMaxQuota)(struct ADAPTER *prAdapter,
		uint16_t u2Port, uint32_t u4MaxQuota);
	void (*pdmaStop)(struct GLUE_INFO *prGlueInfo, u_int8_t enable);
	u_int8_t (*pdmaPollingIdle)(struct GLUE_INFO *prGlueInfo);
	void (*enableInterrupt)(struct ADAPTER *prAdapter);
	void (*disableInterrupt)(struct ADAPTER *prAdapter);
	void (*processTxInterrupt)(struct ADAPTER *prAdapter);
	void (*processRxInterrupt)(struct ADAPTER *prAdapter);
	void (*lowPowerOwnRead)(struct ADAPTER *prAdapter, uint8_t *pfgResult);
	void (*lowPowerOwnSet)(struct ADAPTER *prAdapter, uint8_t *pfgResult);
	void (*lowPowerOwnClear)(struct ADAPTER *prAdapter,
		uint8_t *pfgResult);
	void (*wakeUpWiFi)(struct ADAPTER *prAdapter);
	bool (*isValidRegAccess)(struct ADAPTER *prAdapter,
				 uint32_t u4Register);
	void (*getMailboxStatus)(struct ADAPTER *prAdapter, uint32_t *pu4Val);
	void (*setDummyReg)(struct GLUE_INFO *prGlueInfo);
	void (*checkDummyReg)(struct GLUE_INFO *prGlueInfo);
	void (*tx_ring_ext_ctrl)(struct GLUE_INFO *prGlueInfo,
		struct RTMP_TX_RING *tx_ring, uint32_t index);
	void (*rx_ring_ext_ctrl)(struct GLUE_INFO *prGlueInfo,
		struct RTMP_RX_RING *rx_ring, uint32_t index);
	void (*wfdmaManualPrefetch)(struct GLUE_INFO *prGlueInfo);
	void (*processSoftwareInterrupt)(IN struct ADAPTER *prAdapter);
	void (*softwareInterruptMcu)(IN struct ADAPTER *prAdapter,
		uint32_t intrBitMask);
	void (*hifRst)(struct GLUE_INFO *prGlueInfo);
	void (*initPcieInt)(struct GLUE_INFO *prGlueInfo);
	void (*devReadIntStatus)(struct ADAPTER *prAdapter,
		OUT uint32_t *pu4IntStatus);
	void (*devReadIntStatusNotClr)(struct ADAPTER *prAdapter,
		OUT uint32_t *pu4IntStatus);
	void (*DmaShdlInit)(IN struct ADAPTER *prAdapter);
	uint8_t (*setRxRingHwAddr)(struct RTMP_RX_RING *prRxRing,
		struct BUS_INFO *prBusInfo,
		uint32_t u4SwRingIdx);
	bool (*wfdmaAllocRxRing)(
		struct GLUE_INFO *prGlueInfo,
		bool fgAllocMem);
};

struct HIF_MEM {
	phys_addr_t pa;
	void *va;
};

struct HIF_PREALLOC_MEM {
	struct HIF_MEM rTxDesc[NUM_OF_TX_RING];
	struct HIF_MEM rRxDesc[NUM_OF_RX_RING];
	struct HIF_MEM rTxCmdBuf[TX_RING_SIZE];
#if HIF_RX_PREALLOC_DATA_BUFFER
	struct HIF_MEM rRxDataBuf[RX_RING0_SIZE];
#endif
	struct HIF_MEM rRxEventBuf[RX_RING1_SIZE];
#if HIF_TX_PREALLOC_DATA_BUFFER
	struct HIF_MEM rMsduBuf[HIF_TX_MSDU_TOKEN_NUM];
#endif
	phys_addr_t pucRsvMemBase;
	uint32_t u4RsvMemSize;
	uint32_t u4Offset;
};

/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */
#define axi_resource_start(d, v) \
	((struct mt66xx_chip_info *)(intptr_t)d->axiCsrIoremapAddr)
#define axi_resource_len(d, v) \
	((struct mt66xx_chip_info *)d->axiCsrIoremapLen)
#define axi_name(d)               ("AXI-BUS")
/*need to assign EMI memory in freertos*/
#define WIFIMEMPHYBASE  0x00000000
#define WIFIMEMPHYSIZE  0x00000000
#define IS_ALIGN_X(_x, _value)      (((_value) & (_x-1)) ? FALSE : TRUE)
/*******************************************************************************
 *                   F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */
struct connsys_wf_hif_op *glRegisterBus(void);

void glUnregisterBus(void);

void glSetHifInfo(struct GLUE_INFO *prGlueInfo, unsigned long ulCookie);

void glClearHifInfo(struct GLUE_INFO *prGlueInfo);

uint8_t glBusInit(void *pvData);

void glBusRelease(void *pData);

int32_t glBusSetIrq(void *pvData, void *pfnIsr, void *pvCookie);

void glBusFreeIrq(void *pvData, void *pvCookie);

void glSetPowerState(IN struct GLUE_INFO *prGlueInfo, IN uint32_t ePowerMode);

void glGetDev(void *ctx, /*struct device*/void **dev);

void glGetHifDev(struct GL_HIF_INFO *prHif, /*struct device*/void **dev);

uint8_t glGetBusIntInIRQ(struct GLUE_INFO *prGlueInfo);
/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */
#endif /* _HIF_H */
