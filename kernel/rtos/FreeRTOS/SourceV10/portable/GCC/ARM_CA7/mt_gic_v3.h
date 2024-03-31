/*
 * Copyright (c) 2018 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __MT_GIC_V3_H
#define __MT_GIC_V3_H
#include <printf-stdarg.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef NULL
#define NULL		0U
#endif

#define ICC_IAR1_EL1	sys_reg(3, 0, 12, 12, 0)
#define ICC_EOIR1_EL1	sys_reg(3, 0, 12, 12, 1)
#define ICC_IAR0_EL1	sys_reg(3, 0, 12,  8, 0)
#define ICC_EOIR0_EL1	sys_reg(3, 0, 12,  8, 1)
#define ICC_HPPIR1_EL1	sys_reg(3, 0, 12, 12, 2)
#define GICD_CTLR_ENABLE_GRP0		(1 << 0)
#define GICD_CTLR_ENGRP1NS      	(1 << 1)
#define GICD_CTLR_ENGRP1S   		(1 << 2)
#define GICD_CTLR_ARE       		(1 << 4)
#define GICD_CTLR_ARE_NS                (1 << 5)
#define GICD_CTLR_DS			(1 << 6)
#define GICD_CTLR_E1NWF			(1 << 7)
#define GICD_CTLR_RWP       		(1 << 31)
#define GICR_WAKER_ProcessorSleep	(1 << 1)
#define GICR_WAKER_ChildrenAsleep	(1 << 2)
/* GICD_TYPER bit definitions */
#define IT_LINES_NO_MASK    (0x1f)
#define ENABLE_GRP0		(1 << 0)
#define ENABLE_GRP1		(1 << 1)
/* Mask for the priority field common to all GIC interfaces */
#define GIC_PRI_MASK    0xff
/* GICC_CTLR bit definitions */
#define EOI_MODE_NS		(1 << 10)
#define EOI_MODE_S		(1 << 9)
#define IRQ_BYP_DIS_GRP1	(1 << 8)
#define FIQ_BYP_DIS_GRP1	(1 << 7)
#define IRQ_BYP_DIS_GRP0	(1 << 6)
#define FIQ_BYP_DIS_GRP0	(1 << 5)
#define CBPR			(1 << 4)
#define FIQ_EN			(1 << 3)
#define ACK_CTL			(1 << 2)
/* GICv3 ICC_SRE register bit definitions*/
#define ICC_SRE_EN		(1 << 3)
#define ICC_SRE_SRE		(1 << 0)
/* GICC_IIDR bit masks and shifts */
#define GICC_IIDR_PID_SHIFT 20
#define GICC_IIDR_ARCH_SHIFT 16
#define GICC_IIDR_REV_SHIFT 12
#define GICC_IIDR_IMP_SHIFT	0
#define GICC_IIDR_PID_MASK	0xfff
#define GICC_IIDR_ARCH_MASK	0xf
#define GICC_IIDR_REV_MASK	0xf
#define GICC_IIDR_IMP_MASK	0xfff
#define SZ_64K			(0x00010000)
#define INT_POL_SECCTL_NUM	20
#define NR_INT_POL_CTL		(20)
/* main cpu regs */
#define GICC_CTLR               0x0000
#define GICC_PMR                0x0004
#define GICC_BPR                0x0008
#define GICC_IAR                0x000c
#define GICC_EOIR               0x0010
#define GICC_RPR                0x0014
#define GICC_HPPIR              0x0018
#define GICC_APBR               0x001c
#define GICC_AIAR               0x0020
#define GICC_AEOIR              0x0024
#define GICC_AHPPIR            	0x0028
#define GICC_APR                0x00d0
#define GICC_NSAPR              0x00e0
#define GICC_IIDR               0x00fc
#define GICC_DIR                0x1000
/* distribution regs */
#define GICD_CTLR               0x000
#define GICD_TYPER              0x004
#define GICD_IIDR               0x008
#define GICD_STATUSR      	0x010
#define GICD_SEIR		0x068
#define GICD_IGROUPR            0x080
#define GICD_ISENABLER		0x100
#define GICD_ICENABLER		0x180
#define GICD_ISPENDR		0x200
#define GICD_ICPENDR		0x280
#define GICD_ISACTIVER		0x300
#define GICD_ICACTIVER		0x380
#define GICD_IPRIORITYR		0x400
#define GICD_ITARGETSR		0x800
#define GICD_ICFGR		0xc00
#define GICD_IGRPMODR		0xd00
#define GICD_NSACR		0xe00
#define GICD_SGIR               0xf00
#define GICD_CPENDSGIR		0xf10
#define GICD_SPENDSGIR		0xf20
#define GICD_ROUTE              0x6100
#define GICD_PIDR2		0xFFE8

/*
 * Re-Distributor registers, offsets from RD_base
 */
#define GICR_V3_CTLR                       0x0000
#define GICR_V3_IIDR                       0x0004
#define GICR_V3_TYPER                      0x0008
#define GICR_V3_STATUSR                    0x0010
#define GICR_V3_WAKER                      0x0014
#define GICR_V3_SETLPIR                    0x0040
#define GICR_V3_CLRLPIR                    0x0048
#define GICR_V3_PROPBASER                  0x0070
#define GICR_V3_PENDBASER                  0x0078
#define GICE_V3_IGROUP0			   0x0080
#define GICR_V3_INVLPIR                    0x00A0
#define GICR_V3_INVALLR                    0x00B0
#define GICR_V3_SYNCR                      0x00C0
#define GICR_V3_MOVLPIR                    0x0100
#define GICR_V3_MOVALLR                    0x0110
#define GICE_V3_IGRPMOD0		   0x0d00
#define GIC_V3_PIDR2_ARCH_MASK             0xf0
#define GIC_V3_PIDR2_ARCH_GICv3            0x30
#define GIC_V3_PIDR2_ARCH_GICv4            0x40
#define GICR_V3_WAKER_ProcessorSleep       (1U << 1)
#define GICR_V3_WAKER_ChildrenAsleep       (1U << 2)
/*******************************************************************************
 * GICv3 CPU interface registers & constants
 ******************************************************************************/
/* SCR bit definitions */
#define SCR_NS_BIT                         (1U << 0)
/* ICC_SRE bit definitions*/
#define ICC_SRE_EN_BIT                     (1 << 3)
#define ICC_SRE_DIB_BIT                    (1 << 2)
#define ICC_SRE_DFB_BIT                    (1 << 1)
#define ICC_SRE_SRE_BIT                    (1 << 0)

#define LTRACEF(s) printf(s)

/*
 * Define IRQ code.
 */
#define GIC_SPI_START	32
#define MT_NR_SPI	225
#define NR_IRQ_LINE  (MT_NR_SPI + GIC_SPI_START)

#define	IRQ_TYPE_EDGE_RISING	0x00000001
#define	IRQ_TYPE_EDGE_FALLING	0x00000002
#define	IRQ_TYPE_EDGE_BOTH	(IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define	IRQ_TYPE_LEVEL_HIGH	0x00000004
#define	IRQ_TYPE_LEVEL_LOW	0x00000008

enum {
	/* Ignore cpu_mask and forward interrupt to all CPUs other than the current cpu */
	ARM_GIC_SGI_FLAG_TARGET_FILTER_NOT_SENDER = 0x1,
	/* Ignore cpu_mask and forward interrupt to current CPU only */
	ARM_GIC_SGI_FLAG_TARGET_FILTER_SENDER = 0x2,
	ARM_GIC_SGI_FLAG_TARGET_FILTER_MASK = 0x3,
	/* Only forward the interrupt to CPUs that has the interrupt configured as group 1 (non-secure) */
	ARM_GIC_SGI_FLAG_NS = 0x4,
};

typedef void (*irq_handler_t) (int, void *);

/** @ingroup type_group_scp_intc_ext_struct
 * @brief INTC irq event structure\n
 */
struct irq_desc_t {
/** interrupt event handler */
	irq_handler_t handler;
/** interrupt event device */
	void *dev;
/** interrupt event name */
	const char *name;
/** interrupt event count */
	uint32_t irq_count;
/** interrupt wakeup count */
	uint32_t wakeup_count;
/** initialization times of an interrupt */
	uint32_t init_count;
/** last event handled enter time */
	uint64_t last_enter;
/** last event handled exit time */
	uint64_t last_exit;
/** interrupt event duraiton  */
	uint64_t max_duration;
};

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
int request_irq(uint32_t irq, irq_handler_t handler, uint32_t type, const char *name, void *dev);
void free_irq(uint32_t irq);
uint32_t enable_irq(uint32_t irq);
uint32_t disable_irq(uint32_t irq);
int irq_set_type(uint32_t irq, uint32_t type);

uint32_t mt_gic_icc_primask_read(void);
uint32_t mt_gic_icc_igrpen1_read(void);
uint32_t mt_gic_icc_iar1_read(void);
uint32_t mt_gic_icc_sre_read(void);
uint32_t mt_gic_icc_bpr1_read(void);
void *platform_init_interrupts(void);
void *gic_gicd_get_address(void);
#endif

