/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "adsp_helper.h"
#include <mtk_hifixdsp_common.h>

struct adsp_reserve_mblock {
    enum ADSP_RESERVE_MEM_ID num;
    unsigned long start_phys;
    unsigned long start_virt;
    unsigned long size;
};

static struct adsp_reserve_mblock adsp_reserve_mblock[] = {
    {
        .num = ADSP_A_AUDIO_MEM_ID,
        .start_phys = 0x0,
        .start_virt = 0x0,
        .size = 0xB000,
    },
};

void init_adsp_sysram_reserve_mblock(phys_addr_t pbase,
                                     void *vbase)
{
    enum ADSP_RESERVE_MEM_ID id;
    unsigned long phys_base;
    unsigned long virt_base;
    unsigned long accumlate_memory_size = 0;

    /* assign to each memroy block */
    phys_base = (unsigned long)pbase;
    virt_base = (unsigned long)vbase;
    for (id = 0; id < ADSP_NUMS_SYSRAM_ID; id++) {
        adsp_reserve_mblock[id].start_phys = phys_base +
                                             accumlate_memory_size;
        adsp_reserve_mblock[id].start_virt = virt_base +
                                             accumlate_memory_size;
        accumlate_memory_size += adsp_reserve_mblock[id].size;
    }

    /*
     * the reserved memory should be larger then expected memory
     * or adsp_reserve_mblock does not match dts
     */
    if (accumlate_memory_size > TOTAL_SIZE_SHARED_DRAM_FROM_TAIL) {
        log_hal_error("reserved memory is not enough\n");
        return;
    }

#ifdef ADSP_DEBUG
    for (id = 0; id < ADSP_NUMS_SYSRAM_ID; id++) {
        log_hal_info("[ADSP][id:%d] phys:0x%lx, virt:0x%lx, size:0x%lx\n",
                     id, adsp_get_reserve_sysram_phys(id),
                     adsp_get_reserve_sysram_virt(id),
                     adsp_get_reserve_sysram_size(id));
    }
#endif /* #ifdef ADSP_DEBUG */
}

unsigned long adsp_get_reserve_sysram_phys(enum ADSP_RESERVE_MEM_ID id)
{
    if (id >= ADSP_NUMS_SYSRAM_ID) {
        log_hal_error("[ADSP] no reserve memory for %d", id);
        return 0;
    } else
        return adsp_reserve_mblock[id].start_phys;
}

unsigned long adsp_get_reserve_sysram_virt(enum ADSP_RESERVE_MEM_ID id)
{
    if (id >= ADSP_NUMS_SYSRAM_ID) {
        log_hal_error("[ADSP] no reserve memory for %d", id);
        return 0;
    } else
        return adsp_reserve_mblock[id].start_virt;
}

unsigned long adsp_get_reserve_sysram_size(enum ADSP_RESERVE_MEM_ID id)
{
    if (id >= ADSP_NUMS_SYSRAM_ID) {
        log_hal_error("[ADSP] no reserve memory for %d", id);
        return 0;
    } else
        return adsp_reserve_mblock[id].size;
}

unsigned long adsp_get_shared_dtcm_phys_for_ipc(void)
{
    phys_addr_t phy_base;
    uint32_t ipc_shared_size;
    uint32_t ipc_offset_from_tail;
    struct adsp_chip_info *adsp;

    adsp = get_adsp_chip_data();
    if (!adsp)
        return 0;

    ipc_shared_size = SIZE_SHARED_DTCM_FOR_IPC;
    ipc_offset_from_tail = ipc_shared_size;
    phy_base = adsp->pa_dtcm + adsp->dtcmsize - ipc_offset_from_tail;

    return (unsigned long)phy_base;
}

unsigned long adsp_get_shared_dtcm_virt_for_ipc(void)
{
    unsigned long vbase;
    uint32_t total_shared_size;
    uint32_t ipc_shared_size;
    uint32_t ipc_offset_from_tail;
    struct adsp_chip_info *adsp;

    adsp = get_adsp_chip_data();
    if (!adsp)
        return 0;

    total_shared_size = TOTAL_SIZE_SHARED_DTCM_FROM_TAIL;
    ipc_shared_size = SIZE_SHARED_DTCM_FOR_IPC;
    ipc_offset_from_tail = ipc_shared_size;
    vbase = (unsigned long)adsp->shared_dtcm + total_shared_size -
            ipc_offset_from_tail;

    return vbase;
}

unsigned long adsp_get_shared_dtcm_size_for_ipc(void)
{
    return SIZE_SHARED_DTCM_FOR_IPC;
}

void *adsp_get_shared_sysram_phys2virt(phys_addr_t addr)
{
    phys_addr_t shared_base;
    void *vbase;
    struct adsp_chip_info *adsp;
    uint32_t offset;
    uint32_t shared_size;

    adsp = get_adsp_chip_data();
    if (!adsp)
        return NULL;

    shared_size = TOTAL_SIZE_SHARED_DRAM_FROM_TAIL;
    shared_base = adsp->pa_dram + adsp->dramsize - shared_size;

    if ((addr >= (adsp->pa_dram + adsp->dramsize))
        || (addr < adsp->pa_dram)) {
        log_hal_error("[ADSP] input Paddr: 0x%lx is out of range!", addr);
        return NULL;
    }

    if (addr < shared_base) {
        log_hal_error("[ADSP] input Paddr: 0x%lx is not access for cpu!", addr);
        return NULL;
    }

    offset = addr - shared_base;
    vbase = (void *)((phys_addr_t)adsp->shared_dram + offset);

    return vbase;
}

phys_addr_t adsp_get_shared_sysram_virt2phys(void *addr)
{
    unsigned long phys_base;
    unsigned long virt_base;
    unsigned long virt_addr;
    struct adsp_chip_info *adsp;
    uint32_t offset;
    uint32_t shared_size;

    adsp = get_adsp_chip_data();
    if (!adsp)
        return 0;

    virt_addr = (unsigned long)addr;
    virt_base = (unsigned long)adsp->shared_dram;
    shared_size = TOTAL_SIZE_SHARED_DRAM_FROM_TAIL;
    if ((virt_addr >= (virt_base + shared_size))
        || (virt_addr < virt_base)) {
        log_hal_error("[ADSP] input Vaddr: %p is out of range!", addr);
        return 0;
    }

    offset = virt_addr - virt_base;
    phys_base = adsp->pa_dram + adsp->dramsize - shared_size;

    return (phys_base + offset);
}

static int is_cpu_dtcm_phys_addr(phys_addr_t addr)
{
    struct adsp_chip_info *adsp;

    adsp = get_adsp_chip_data();

    if ((addr < (adsp->pa_dtcm + adsp->dtcmsize))
        && (addr >= adsp->pa_dtcm))
        return 1;

    return 0;
}

static int is_dsp_dtcm_phys_addr(phys_addr_t addr)
{
    struct adsp_chip_info *adsp;

    adsp = get_adsp_chip_data();

    if ((addr < (DTCM_PHYS_BASE_FROM_DSP_VIEW + adsp->dtcmsize))
        && (addr >= DTCM_PHYS_BASE_FROM_DSP_VIEW))
        return 1;

    return 0;
}

static int is_cpu_sysram_phys_addr(phys_addr_t addr)
{
    struct adsp_chip_info *adsp;

    adsp = get_adsp_chip_data();

    if ((addr < (adsp->pa_dram + adsp->dramsize))
        && (addr >= adsp->pa_dram))
        return 1;

    return 0;
}

static int is_dsp_sysram_phys_addr(phys_addr_t addr)
{
    struct adsp_chip_info *adsp;

    adsp = get_adsp_chip_data();

    if ((addr < (DRAM_PHYS_BASE_FROM_DSP_VIEW + adsp->dramsize))
        && (addr >= DRAM_PHYS_BASE_FROM_DSP_VIEW))
        return 1;

    return 0;
}

phys_addr_t adsp_hal_phys_addr_cpu2dsp(phys_addr_t addr)
{
    phys_addr_t paddr;
    phys_addr_t base;
    struct adsp_chip_info *adsp;
    uint32_t offset;

    adsp = get_adsp_chip_data();
    if (!adsp)
        return 0;

    /*
     * Need to check 'paddr' range for different sections.
     */
    if (is_cpu_dtcm_phys_addr(addr)) {
        base = DTCM_PHYS_BASE_FROM_DSP_VIEW;
        offset = addr - adsp->pa_dtcm;
        paddr = base + offset;
    } else if (is_cpu_sysram_phys_addr(addr)) {
        base = DRAM_PHYS_BASE_FROM_DSP_VIEW;
        offset = addr - adsp->pa_dram;
        paddr = base + offset;
    } else {
        log_hal_error("[ADSP] CPU PhyAddr: 0x%lx is out of range!", addr);
        return 0;
    }

    return paddr;
}

phys_addr_t adsp_hal_phys_addr_dsp2cpu(phys_addr_t addr)
{
    phys_addr_t paddr;
    phys_addr_t base;
    struct adsp_chip_info *adsp;
    uint32_t offset;

    adsp = get_adsp_chip_data();
    if (!adsp)
        return 0;

    /*
     * Need to check 'paddr' range for different sections.
     */
    if (is_dsp_dtcm_phys_addr(addr)) {
        base = adsp->pa_dtcm;
        offset = addr - DTCM_PHYS_BASE_FROM_DSP_VIEW;
        paddr = base + offset;
    } else if (is_dsp_sysram_phys_addr(addr)) {
        base = adsp->pa_dram;
        offset = addr - DRAM_PHYS_BASE_FROM_DSP_VIEW;
        paddr = base + offset;
    } else {
        log_hal_error("[ADSP] DSP PhyAddr: 0x%lx is out of range!", addr);
        return 0;
    }

    return paddr;
}

