#ifndef __DDRPHY_NAO_REGS_H__
#define __DDRPHY_NAO_REGS_H__
#include "dramc_register.h"

//#define DDRPHY_NAO_BASE_ADDRESS 0x1022E000
#define DDRPHY_NAO_BASE_ADDRESS Channel_A_PHY_NAO_BASE_VIRTUAL

#define DDRPHY_MISC_STA_EXTLB0                         (DDRPHY_NAO_BASE_ADDRESS + 0x00000)
#define MISC_STA_EXTLB0_STA_EXTLB_DONE                     Fld(32, 0) //[31:0]

#define DDRPHY_MISC_STA_EXTLB1                         (DDRPHY_NAO_BASE_ADDRESS + 0x00004)
#define MISC_STA_EXTLB1_STA_EXTLB_FAIL                     Fld(32, 0) //[31:0]

#define DDRPHY_MISC_STA_EXTLB2                         (DDRPHY_NAO_BASE_ADDRESS + 0x00008)
#define MISC_STA_EXTLB2_STA_EXTLB_DBG_INFO                 Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO0                     (DDRPHY_NAO_BASE_ADDRESS + 0x00080)
#define MISC_DQ_RXDLY_TRRO0_DVS_RKX_BX_SW_LAG_CNT_OUT_B0   Fld(8, 0) //[7:0]
#define MISC_DQ_RXDLY_TRRO0_DVS_RKX_BX_SW_LEAD_CNT_OUT_B0  Fld(8, 8) //[15:8]
#define MISC_DQ_RXDLY_TRRO0_DVS_RKX_BX_SW_LAG_CNT_OUT_B1   Fld(8, 16) //[23:16]
#define MISC_DQ_RXDLY_TRRO0_DVS_RKX_BX_SW_LEAD_CNT_OUT_B1  Fld(8, 24) //[31:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO1                     (DDRPHY_NAO_BASE_ADDRESS + 0x00084)
#define MISC_DQ_RXDLY_TRRO1_DVS_RKX_BX_SW_LAG_CNT_OUT_B2   Fld(8, 0) //[7:0]
#define MISC_DQ_RXDLY_TRRO1_DVS_RKX_BX_SW_LEAD_CNT_OUT_B2  Fld(8, 8) //[15:8]
#define MISC_DQ_RXDLY_TRRO1_DVS_RKX_BX_SW_LAG_CNT_OUT_B3   Fld(8, 16) //[23:16]
#define MISC_DQ_RXDLY_TRRO1_DVS_RKX_BX_SW_LEAD_CNT_OUT_B3  Fld(8, 24) //[31:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO2                     (DDRPHY_NAO_BASE_ADDRESS + 0x00088)
#define MISC_DQ_RXDLY_TRRO2_DVS_RKX_BX_SW_LAG_CNT_OUT_B4   Fld(8, 0) //[7:0]
#define MISC_DQ_RXDLY_TRRO2_DVS_RKX_BX_SW_LEAD_CNT_OUT_B4  Fld(8, 8) //[15:8]
#define MISC_DQ_RXDLY_TRRO2_DVS_RKX_BX_SW_LAG_CNT_OUT_B5   Fld(8, 16) //[23:16]
#define MISC_DQ_RXDLY_TRRO2_DVS_RKX_BX_SW_LEAD_CNT_OUT_B5  Fld(8, 24) //[31:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO3                     (DDRPHY_NAO_BASE_ADDRESS + 0x0008C)
#define MISC_DQ_RXDLY_TRRO3_DVS_RKX_BX_SW_LAG_CNT_OUT_B6   Fld(8, 0) //[7:0]
#define MISC_DQ_RXDLY_TRRO3_DVS_RKX_BX_SW_LEAD_CNT_OUT_B6  Fld(8, 8) //[15:8]
#define MISC_DQ_RXDLY_TRRO3_DVS_RKX_BX_SW_LAG_CNT_OUT_B7   Fld(8, 16) //[23:16]
#define MISC_DQ_RXDLY_TRRO3_DVS_RKX_BX_SW_LEAD_CNT_OUT_B7  Fld(8, 24) //[31:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO4                     (DDRPHY_NAO_BASE_ADDRESS + 0x00090)
#define MISC_DQ_RXDLY_TRRO4_DVS_RKX_BX_LEAD_LAG_CNT_OUT_B0 Fld(8, 0) //[7:0]
#define MISC_DQ_RXDLY_TRRO4_DVS_RKX_BX_LEAD_LAG_CNT_OUT_B1 Fld(8, 8) //[15:8]
#define MISC_DQ_RXDLY_TRRO4_DVS_RKX_BX_LEAD_LAG_CNT_OUT_B2 Fld(8, 16) //[23:16]
#define MISC_DQ_RXDLY_TRRO4_DVS_RKX_BX_LEAD_LAG_CNT_OUT_B3 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO5                     (DDRPHY_NAO_BASE_ADDRESS + 0x00094)
#define MISC_DQ_RXDLY_TRRO5_DVS_RKX_BX_LEAD_LAG_CNT_OUT_B4 Fld(8, 0) //[7:0]
#define MISC_DQ_RXDLY_TRRO5_DVS_RKX_BX_LEAD_LAG_CNT_OUT_B5 Fld(8, 8) //[15:8]
#define MISC_DQ_RXDLY_TRRO5_DVS_RKX_BX_LEAD_LAG_CNT_OUT_B6 Fld(8, 16) //[23:16]
#define MISC_DQ_RXDLY_TRRO5_DVS_RKX_BX_LEAD_LAG_CNT_OUT_B7 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO6                     (DDRPHY_NAO_BASE_ADDRESS + 0x00098)
#define MISC_DQ_RXDLY_TRRO6_DVS_RKX_BX_SW_LAG_CNT_OUT_DQM0 Fld(8, 0) //[7:0]
#define MISC_DQ_RXDLY_TRRO6_DVS_RKX_BX_SW_LEAD_CNT_OUT_DQM0 Fld(8, 8) //[15:8]
#define MISC_DQ_RXDLY_TRRO6_DVS_RKX_BX_LEAD_LAG_CNT_OUT_DQM0 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO7                     (DDRPHY_NAO_BASE_ADDRESS + 0x0009C)
#define MISC_DQ_RXDLY_TRRO7_DVS_RK0_B0_SW_UP_DONE          Fld(1, 0) //[0:0]
#define MISC_DQ_RXDLY_TRRO7_DVS_RK0_B1_SW_UP_DONE          Fld(1, 4) //[4:4]
#define MISC_DQ_RXDLY_TRRO7_DVS_RK1_B0_SW_UP_DONE          Fld(1, 8) //[8:8]
#define MISC_DQ_RXDLY_TRRO7_DVS_RK1_B1_SW_UP_DONE          Fld(1, 12) //[12:12]
#define MISC_DQ_RXDLY_TRRO7_DVS_RK2_B0_SW_UP_DONE          Fld(1, 16) //[16:16]
#define MISC_DQ_RXDLY_TRRO7_DVS_RK2_B1_SW_UP_DONE          Fld(1, 20) //[20:20]

#define DDRPHY_MISC_DQ_RXDLY_TRRO8                     (DDRPHY_NAO_BASE_ADDRESS + 0x000A0)
#define MISC_DQ_RXDLY_TRRO8_DVS_RKX_BX_TH_CNT_OUT_B0       Fld(9, 0) //[8:0]
#define MISC_DQ_RXDLY_TRRO8_DVS_RKX_BX_TH_CNT_OUT_B1       Fld(9, 16) //[24:16]

#define DDRPHY_MISC_DQ_RXDLY_TRRO9                     (DDRPHY_NAO_BASE_ADDRESS + 0x000A4)
#define MISC_DQ_RXDLY_TRRO9_DVS_RKX_BX_TH_CNT_OUT_B2       Fld(9, 0) //[8:0]
#define MISC_DQ_RXDLY_TRRO9_DVS_RKX_BX_TH_CNT_OUT_B3       Fld(9, 16) //[24:16]

#define DDRPHY_MISC_DQ_RXDLY_TRRO10                    (DDRPHY_NAO_BASE_ADDRESS + 0x000A8)
#define MISC_DQ_RXDLY_TRRO10_DVS_RKX_BX_TH_CNT_OUT_B4      Fld(9, 0) //[8:0]
#define MISC_DQ_RXDLY_TRRO10_DVS_RKX_BX_TH_CNT_OUT_B5      Fld(9, 16) //[24:16]

#define DDRPHY_MISC_DQ_RXDLY_TRRO11                    (DDRPHY_NAO_BASE_ADDRESS + 0x000AC)
#define MISC_DQ_RXDLY_TRRO11_DVS_RKX_BX_TH_CNT_OUT_B6      Fld(9, 0) //[8:0]
#define MISC_DQ_RXDLY_TRRO11_DVS_RKX_BX_TH_CNT_OUT_B7      Fld(9, 16) //[24:16]

#define DDRPHY_MISC_DQ_RXDLY_TRRO12                    (DDRPHY_NAO_BASE_ADDRESS + 0x000B0)
#define MISC_DQ_RXDLY_TRRO12_DVS_RKX_BX_TH_CNT_OUT_DQM0    Fld(9, 0) //[8:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO13                    (DDRPHY_NAO_BASE_ADDRESS + 0x000B4)
#define MISC_DQ_RXDLY_TRRO13_DA_RK0_DQX_B0_R_DLY           Fld(6, 0) //[5:0]
#define MISC_DQ_RXDLY_TRRO13_DA_RK0_DQS0_R_DLY             Fld(7, 8) //[14:8]
#define MISC_DQ_RXDLY_TRRO13_DA_RK0_DQX_B1_R_DLY           Fld(6, 16) //[21:16]
#define MISC_DQ_RXDLY_TRRO13_DA_RK0_DQS1_R_DLY             Fld(7, 24) //[30:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO14                    (DDRPHY_NAO_BASE_ADDRESS + 0x000B8)
#define MISC_DQ_RXDLY_TRRO14_DA_RK1_DQX_B0_R_DLY           Fld(6, 0) //[5:0]
#define MISC_DQ_RXDLY_TRRO14_DA_RK1_DQS0_R_DLY             Fld(7, 8) //[14:8]
#define MISC_DQ_RXDLY_TRRO14_DA_RK1_DQX_B1_R_DLY           Fld(6, 16) //[21:16]
#define MISC_DQ_RXDLY_TRRO14_DA_RK1_DQS1_R_DLY             Fld(7, 24) //[30:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO15                    (DDRPHY_NAO_BASE_ADDRESS + 0x000BC)
#define MISC_DQ_RXDLY_TRRO15_DA_RK2_DQX_B0_R_DLY           Fld(6, 0) //[5:0]
#define MISC_DQ_RXDLY_TRRO15_DA_RK2_DQS0_R_DLY             Fld(7, 8) //[14:8]
#define MISC_DQ_RXDLY_TRRO15_DA_RK2_DQX_B1_R_DLY           Fld(6, 16) //[21:16]
#define MISC_DQ_RXDLY_TRRO15_DA_RK2_DQS1_R_DLY             Fld(7, 24) //[30:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO16                    (DDRPHY_NAO_BASE_ADDRESS + 0x000C0)
#define MISC_DQ_RXDLY_TRRO16_DVS_RXDLY_STS_ERR_CNT_ALL     Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO17                    (DDRPHY_NAO_BASE_ADDRESS + 0x000C4)
#define MISC_DQ_RXDLY_TRRO17_DVS_RXDLY_STS_ERR_CNT_ALL_47_32 Fld(16, 0) //[15:0]
#define MISC_DQ_RXDLY_TRRO17_PBYTE_LEADLAG_STUCK_B0        Fld(1, 16) //[16:16]
#define MISC_DQ_RXDLY_TRRO17_PBYTE_LEADLAG_STUCK_B1        Fld(1, 24) //[24:24]

#define DDRPHY_MISC_DQ_RXDLY_TRRO18                    (DDRPHY_NAO_BASE_ADDRESS + 0x000C8)
#define MISC_DQ_RXDLY_TRRO18_RXDLY_DBG_MON_VALID           Fld(1, 0) //[0:0]
#define MISC_DQ_RXDLY_TRRO18_RXDLY_RK0_FAIL_LAT            Fld(1, 1) //[1:1]
#define MISC_DQ_RXDLY_TRRO18_RXDLY_RK1_FAIL_LAT            Fld(1, 2) //[2:2]
#define MISC_DQ_RXDLY_TRRO18_RXDLY_RK2_FAIL_LAT            Fld(1, 3) //[3:3]
#define MISC_DQ_RXDLY_TRRO18_DFS_SHU_GP_FAIL_LAT           Fld(2, 4) //[5:4]

#define DDRPHY_MISC_DQ_RXDLY_TRRO19                    (DDRPHY_NAO_BASE_ADDRESS + 0x000CC)
#define MISC_DQ_RXDLY_TRRO19_RESERVED_0X00C                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO20                    (DDRPHY_NAO_BASE_ADDRESS + 0x000D0)
#define MISC_DQ_RXDLY_TRRO20_RESERVED_0X0D0                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO21                    (DDRPHY_NAO_BASE_ADDRESS + 0x000D4)
#define MISC_DQ_RXDLY_TRRO21_RESERVED_0X0D4                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO22                    (DDRPHY_NAO_BASE_ADDRESS + 0x000D8)
#define MISC_DQ_RXDLY_TRRO22_RESERVED_0X0D8                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO23                    (DDRPHY_NAO_BASE_ADDRESS + 0x000DC)
#define MISC_DQ_RXDLY_TRRO23_RESERVED_0X0DC                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO24                    (DDRPHY_NAO_BASE_ADDRESS + 0x000E0)
#define MISC_DQ_RXDLY_TRRO24_RESERVED_0X0E0                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO25                    (DDRPHY_NAO_BASE_ADDRESS + 0x000E4)
#define MISC_DQ_RXDLY_TRRO25_RESERVED_0X0E4                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO26                    (DDRPHY_NAO_BASE_ADDRESS + 0x000E8)
#define MISC_DQ_RXDLY_TRRO26_RESERVED_0X0E8                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO27                    (DDRPHY_NAO_BASE_ADDRESS + 0x000EC)
#define MISC_DQ_RXDLY_TRRO27_RESERVED_0X0EC                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO28                    (DDRPHY_NAO_BASE_ADDRESS + 0x000F0)
#define MISC_DQ_RXDLY_TRRO28_RESERVED_0X0F0                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO29                    (DDRPHY_NAO_BASE_ADDRESS + 0x000F4)
#define MISC_DQ_RXDLY_TRRO29_RESERVED_0X0F4                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO30                    (DDRPHY_NAO_BASE_ADDRESS + 0x000F8)
#define MISC_DQ_RXDLY_TRRO30_RESERVED_0X0F8                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQ_RXDLY_TRRO31                    (DDRPHY_NAO_BASE_ADDRESS + 0x000FC)
#define MISC_DQ_RXDLY_TRRO31_RESERVED_0X0FC                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO0                     (DDRPHY_NAO_BASE_ADDRESS + 0x00100)
#define MISC_CA_RXDLY_TRRO0_DVS_RKX_CA_SW_LAG_CNT_OUT_CA0  Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO0_DVS_RKX_CA_SW_LEAD_CNT_OUT_CA0 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO0_DVS_RKX_CA_SW_LAG_CNT_OUT_CA1  Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO0_DVS_RKX_CA_SW_LEAD_CNT_OUT_CA1 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO1                     (DDRPHY_NAO_BASE_ADDRESS + 0x00104)
#define MISC_CA_RXDLY_TRRO1_DVS_RKX_CA_SW_LAG_CNT_OUT_CA2  Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO1_DVS_RKX_CA_SW_LEAD_CNT_OUT_CA2 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO1_DVS_RKX_CA_SW_LAG_CNT_OUT_CA3  Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO1_DVS_RKX_CA_SW_LEAD_CNT_OUT_CA3 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO2                     (DDRPHY_NAO_BASE_ADDRESS + 0x00108)
#define MISC_CA_RXDLY_TRRO2_DVS_RKX_CA_SW_LAG_CNT_OUT_CA4  Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO2_DVS_RKX_CA_SW_LEAD_CNT_OUT_CA4 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO2_DVS_RKX_CA_SW_LAG_CNT_OUT_CA5  Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO2_DVS_RKX_CA_SW_LEAD_CNT_OUT_CA5 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO3                     (DDRPHY_NAO_BASE_ADDRESS + 0x0010C)
#define MISC_CA_RXDLY_TRRO3_DVS_RKX_CA_SW_LAG_CNT_OUT_CKE0 Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO3_DVS_RKX_CA_SW_LEAD_CNT_OUT_CKE0 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO3_DVS_RKX_CA_SW_LAG_CNT_OUT_CKE1 Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO3_DVS_RKX_CA_SW_LEAD_CNT_OUT_CKE1 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO4                     (DDRPHY_NAO_BASE_ADDRESS + 0x00110)
#define MISC_CA_RXDLY_TRRO4_DVS_RKX_CA_SW_LAG_CNT_OUT_CKE2 Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO4_DVS_RKX_CA_SW_LEAD_CNT_OUT_CKE2 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO4_DVS_RKX_CA_SW_LAG_CNT_OUT_CS0  Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO4_DVS_RKX_CA_SW_LEAD_CNT_OUT_CS0 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO5                     (DDRPHY_NAO_BASE_ADDRESS + 0x00114)
#define MISC_CA_RXDLY_TRRO5_DVS_RKX_CA_SW_LAG_CNT_OUT_CS1  Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO5_DVS_RKX_CA_SW_LEAD_CNT_OUT_CS1 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO5_DVS_RKX_CA_SW_LAG_CNT_OUT_CS2  Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO5_DVS_RKX_CA_SW_LEAD_CNT_OUT_CS2 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO6                     (DDRPHY_NAO_BASE_ADDRESS + 0x00118)
#define MISC_CA_RXDLY_TRRO6_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CA0 Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO6_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CA1 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO6_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CA2 Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO6_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CA3 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO7                     (DDRPHY_NAO_BASE_ADDRESS + 0x0011C)
#define MISC_CA_RXDLY_TRRO7_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CA4 Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO7_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CA5 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO7_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CKE0 Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO7_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CKE1 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO8                     (DDRPHY_NAO_BASE_ADDRESS + 0x00120)
#define MISC_CA_RXDLY_TRRO8_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CKE2 Fld(8, 0) //[7:0]
#define MISC_CA_RXDLY_TRRO8_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CS0 Fld(8, 8) //[15:8]
#define MISC_CA_RXDLY_TRRO8_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CS1 Fld(8, 16) //[23:16]
#define MISC_CA_RXDLY_TRRO8_DVS_RKX_CA_LEAD_LAG_CNT_OUT_CS2 Fld(8, 24) //[31:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO9                     (DDRPHY_NAO_BASE_ADDRESS + 0x00124)
#define MISC_CA_RXDLY_TRRO9_DVS_RK0_CA_SW_UP_DONE          Fld(1, 0) //[0:0]
#define MISC_CA_RXDLY_TRRO9_DVS_RK1_CA_SW_UP_DONE          Fld(1, 8) //[8:8]
#define MISC_CA_RXDLY_TRRO9_DVS_RK2_CA_SW_UP_DONE          Fld(1, 16) //[16:16]

#define DDRPHY_MISC_CA_RXDLY_TRRO10                    (DDRPHY_NAO_BASE_ADDRESS + 0x00128)
#define MISC_CA_RXDLY_TRRO10_DVS_RKX_CA_TH_CNT_OUT_CA0     Fld(9, 0) //[8:0]
#define MISC_CA_RXDLY_TRRO10_DVS_RKX_CA_TH_CNT_OUT_CA1     Fld(9, 16) //[24:16]

#define DDRPHY_MISC_CA_RXDLY_TRRO11                    (DDRPHY_NAO_BASE_ADDRESS + 0x0012C)
#define MISC_CA_RXDLY_TRRO11_DVS_RKX_CA_TH_CNT_OUT_CA2     Fld(9, 0) //[8:0]
#define MISC_CA_RXDLY_TRRO11_DVS_RKX_CA_TH_CNT_OUT_CA3     Fld(9, 16) //[24:16]

#define DDRPHY_MISC_CA_RXDLY_TRRO12                    (DDRPHY_NAO_BASE_ADDRESS + 0x00130)
#define MISC_CA_RXDLY_TRRO12_DVS_RKX_CA_TH_CNT_OUT_CA4     Fld(9, 0) //[8:0]
#define MISC_CA_RXDLY_TRRO12_DVS_RKX_CA_TH_CNT_OUT_CA5     Fld(9, 16) //[24:16]

#define DDRPHY_MISC_CA_RXDLY_TRRO13                    (DDRPHY_NAO_BASE_ADDRESS + 0x00134)
#define MISC_CA_RXDLY_TRRO13_DVS_RKX_CA_TH_CNT_OUT_CKE0    Fld(9, 0) //[8:0]
#define MISC_CA_RXDLY_TRRO13_DVS_RKX_CA_TH_CNT_OUT_CKE1    Fld(9, 16) //[24:16]

#define DDRPHY_MISC_CA_RXDLY_TRRO14                    (DDRPHY_NAO_BASE_ADDRESS + 0x00138)
#define MISC_CA_RXDLY_TRRO14_DVS_RKX_CA_TH_CNT_OUT_CKE2    Fld(9, 0) //[8:0]
#define MISC_CA_RXDLY_TRRO14_DVS_RKX_CA_TH_CNT_OUT_CS0     Fld(9, 16) //[24:16]

#define DDRPHY_MISC_CA_RXDLY_TRRO15                    (DDRPHY_NAO_BASE_ADDRESS + 0x0013C)
#define MISC_CA_RXDLY_TRRO15_DVS_RKX_CA_TH_CNT_OUT_CS1     Fld(9, 0) //[8:0]
#define MISC_CA_RXDLY_TRRO15_DVS_RKX_CA_TH_CNT_OUT_CS2     Fld(9, 16) //[24:16]

#define DDRPHY_MISC_CA_RXDLY_TRRO16                    (DDRPHY_NAO_BASE_ADDRESS + 0x00140)
#define MISC_CA_RXDLY_TRRO16_DA_RK0_CAX_CA_R_DLY           Fld(6, 0) //[5:0]
#define MISC_CA_RXDLY_TRRO16_DA_RK0_CLK_R_DLY              Fld(8, 8) //[15:8]

#define DDRPHY_MISC_CA_RXDLY_TRRO17                    (DDRPHY_NAO_BASE_ADDRESS + 0x00144)
#define MISC_CA_RXDLY_TRRO17_DA_RK1_CAX_CA_R_DLY           Fld(6, 0) //[5:0]
#define MISC_CA_RXDLY_TRRO17_DA_RK1_CLK_R_DLY              Fld(8, 8) //[15:8]

#define DDRPHY_MISC_CA_RXDLY_TRRO18                    (DDRPHY_NAO_BASE_ADDRESS + 0x00148)
#define MISC_CA_RXDLY_TRRO18_DA_RK2_CAX_CA_R_DLY           Fld(6, 0) //[5:0]
#define MISC_CA_RXDLY_TRRO18_DA_RK2_CLK_R_DLY              Fld(8, 8) //[15:8]

#define DDRPHY_MISC_CA_RXDLY_TRRO19                    (DDRPHY_NAO_BASE_ADDRESS + 0x0014C)
#define MISC_CA_RXDLY_TRRO19_DVS_RXDLY_STS_ERR_CNT_ALL_CA  Fld(24, 0) //[23:0]
#define MISC_CA_RXDLY_TRRO19_PBYTE_LEADLAG_STUCK_CA        Fld(1, 24) //[24:24]

#define DDRPHY_MISC_CA_RXDLY_TRRO20                    (DDRPHY_NAO_BASE_ADDRESS + 0x00150)
#define MISC_CA_RXDLY_TRRO20_RESERVED_0X150                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO21                    (DDRPHY_NAO_BASE_ADDRESS + 0x00154)
#define MISC_CA_RXDLY_TRRO21_RESERVED_0X154                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO22                    (DDRPHY_NAO_BASE_ADDRESS + 0x00158)
#define MISC_CA_RXDLY_TRRO22_RESERVED_0X158                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO23                    (DDRPHY_NAO_BASE_ADDRESS + 0x0015C)
#define MISC_CA_RXDLY_TRRO23_RESERVED_0X15C                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO24                    (DDRPHY_NAO_BASE_ADDRESS + 0x00160)
#define MISC_CA_RXDLY_TRRO24_RESERVED_0X160                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO25                    (DDRPHY_NAO_BASE_ADDRESS + 0x00164)
#define MISC_CA_RXDLY_TRRO25_RESERVED_0X164                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO26                    (DDRPHY_NAO_BASE_ADDRESS + 0x00168)
#define MISC_CA_RXDLY_TRRO26_RESERVED_0X168                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO27                    (DDRPHY_NAO_BASE_ADDRESS + 0x0016C)
#define MISC_CA_RXDLY_TRRO27_RESERVED_0X16C                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO28                    (DDRPHY_NAO_BASE_ADDRESS + 0x00170)
#define MISC_CA_RXDLY_TRRO28_RESERVED_0X170                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO29                    (DDRPHY_NAO_BASE_ADDRESS + 0x00174)
#define MISC_CA_RXDLY_TRRO29_RESERVED_0X174                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO30                    (DDRPHY_NAO_BASE_ADDRESS + 0x00178)
#define MISC_CA_RXDLY_TRRO30_RESERVED_0X178                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CA_RXDLY_TRRO31                    (DDRPHY_NAO_BASE_ADDRESS + 0x0017C)
#define MISC_CA_RXDLY_TRRO31_RESERVED_0X17C                Fld(32, 0) //[31:0]

#define DDRPHY_MISC_DQO1                               (DDRPHY_NAO_BASE_ADDRESS + 0x00180)
#define MISC_DQO1_DQO1_RO                                  Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CAO1                               (DDRPHY_NAO_BASE_ADDRESS + 0x00184)
#define MISC_CAO1_RA0_O1                                   Fld(1, 0) //[0:0]
#define MISC_CAO1_RA1_O1                                   Fld(1, 1) //[1:1]
#define MISC_CAO1_RA2_O1                                   Fld(1, 2) //[2:2]
#define MISC_CAO1_RA3_O1                                   Fld(1, 3) //[3:3]
#define MISC_CAO1_RA4_O1                                   Fld(1, 4) //[4:4]
#define MISC_CAO1_RA5_O1                                   Fld(1, 5) //[5:5]
#define MISC_CAO1_RA6_O1                                   Fld(1, 6) //[6:6]
#define MISC_CAO1_RA7_O1                                   Fld(1, 7) //[7:7]
#define MISC_CAO1_RA8_O1                                   Fld(1, 8) //[8:8]
#define MISC_CAO1_RA9_O1                                   Fld(1, 9) //[9:9]
#define MISC_CAO1_CKEO1_RO                                 Fld(1, 10) //[10:10]
#define MISC_CAO1_CKE1O1_RO                                Fld(1, 11) //[11:11]
#define MISC_CAO1_CKE2O1_RO                                Fld(1, 12) //[12:12]
#define MISC_CAO1_CSO1_RO                                  Fld(1, 13) //[13:13]
#define MISC_CAO1_CS1O1_RO                                 Fld(1, 14) //[14:14]
#define MISC_CAO1_CS2O1_RO                                 Fld(1, 15) //[15:15]
#define MISC_CAO1_RESETO1_RO                               Fld(1, 16) //[16:16]
#define MISC_CAO1_DQM0O1_RO                                Fld(1, 24) //[24:24]
#define MISC_CAO1_DQM1O1_RO                                Fld(1, 25) //[25:25]
#define MISC_CAO1_DQM2O1_RO                                Fld(1, 26) //[26:26]
#define MISC_CAO1_DQM3O1_RO                                Fld(1, 27) //[27:27]

#define DDRPHY_MISC_AD_RX_DQ_O1                        (DDRPHY_NAO_BASE_ADDRESS + 0x00188)
#define MISC_AD_RX_DQ_O1_AD_RX_ARDQ_O1_B0                  Fld(8, 0) //[7:0]
#define MISC_AD_RX_DQ_O1_AD_RX_ARDQM0_O1_B0                Fld(1, 8) //[8:8]
#define MISC_AD_RX_DQ_O1_AD_RX_ARDQ_O1_B1                  Fld(8, 16) //[23:16]
#define MISC_AD_RX_DQ_O1_AD_RX_ARDQM0_O1_B1                Fld(1, 24) //[24:24]

#define DDRPHY_MISC_AD_RX_CMD_O1                       (DDRPHY_NAO_BASE_ADDRESS + 0x0018C)
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA0_O1                   Fld(1, 0) //[0:0]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA1_O1                   Fld(1, 1) //[1:1]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA2_O1                   Fld(1, 2) //[2:2]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA3_O1                   Fld(1, 3) //[3:3]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA4_O1                   Fld(1, 4) //[4:4]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA5_O1                   Fld(1, 5) //[5:5]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA6_O1                   Fld(1, 6) //[6:6]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA7_O1                   Fld(1, 7) //[7:7]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA8_O1                   Fld(1, 8) //[8:8]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCA9_O1                   Fld(1, 9) //[9:9]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCKE0_O1                  Fld(1, 10) //[10:10]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCKE1_O1                  Fld(1, 11) //[11:11]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCKE2_O1                  Fld(1, 12) //[12:12]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCS0_O1                   Fld(1, 13) //[13:13]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCS1_O1                   Fld(1, 14) //[14:14]
#define MISC_AD_RX_CMD_O1_AD_RX_ARCS2_O1                   Fld(1, 15) //[15:15]

#define DDRPHY_MISC_PHY_RGS_DQ                         (DDRPHY_NAO_BASE_ADDRESS + 0x00190)
#define MISC_PHY_RGS_DQ_RGS_ARDQ_OFFSET_FLAG_B0            Fld(8, 0) //[7:0]
#define MISC_PHY_RGS_DQ_RGS_ARDQM0_OFFSET_FLAG_B0          Fld(1, 8) //[8:8]
#define MISC_PHY_RGS_DQ_RGS_RX_ARDQS0_RDY_EYE_B0           Fld(1, 9) //[9:9]
#define MISC_PHY_RGS_DQ_RGS_ARDQ_OFFSET_FLAG_B1            Fld(8, 16) //[23:16]
#define MISC_PHY_RGS_DQ_RGS_ARDQM0_OFFSET_FLAG_B1          Fld(1, 24) //[24:24]
#define MISC_PHY_RGS_DQ_RGS_RX_ARDQS0_RDY_EYE_B1           Fld(1, 25) //[25:25]
#define MISC_PHY_RGS_DQ_DA_RPHYPLLGP_CK_SEL                Fld(1, 31) //[31:31]

#define DDRPHY_MISC_PHY_RGS_CMD                        (DDRPHY_NAO_BASE_ADDRESS + 0x00194)
#define MISC_PHY_RGS_CMD_RGS_ARCA0_OFFSET_FLAG             Fld(1, 0) //[0:0]
#define MISC_PHY_RGS_CMD_RGS_ARCA1_OFFSET_FLAG             Fld(1, 1) //[1:1]
#define MISC_PHY_RGS_CMD_RGS_ARCA2_OFFSET_FLAG             Fld(1, 2) //[2:2]
#define MISC_PHY_RGS_CMD_RGS_ARCA3_OFFSET_FLAG             Fld(1, 3) //[3:3]
#define MISC_PHY_RGS_CMD_RGS_ARCA4_OFFSET_FLAG             Fld(1, 4) //[4:4]
#define MISC_PHY_RGS_CMD_RGS_ARCA5_OFFSET_FLAG             Fld(1, 5) //[5:5]
#define MISC_PHY_RGS_CMD_RGS_ARCA6_OFFSET_FLAG             Fld(1, 6) //[6:6]
#define MISC_PHY_RGS_CMD_RGS_ARCA7_OFFSET_FLAG             Fld(1, 7) //[7:7]
#define MISC_PHY_RGS_CMD_RGS_ARCA8_OFFSET_FLAG             Fld(1, 8) //[8:8]
#define MISC_PHY_RGS_CMD_RGS_ARCA9_OFFSET_FLAG             Fld(1, 9) //[9:9]
#define MISC_PHY_RGS_CMD_RGS_ARCKE0_OFFSET_FLAG            Fld(1, 10) //[10:10]
#define MISC_PHY_RGS_CMD_RGS_ARCKE1_OFFSET_FLAG            Fld(1, 11) //[11:11]
#define MISC_PHY_RGS_CMD_RGS_ARCKE2_OFFSET_FLAG            Fld(1, 12) //[12:12]
#define MISC_PHY_RGS_CMD_RGS_ARCS0_OFFSET_FLAG             Fld(1, 13) //[13:13]
#define MISC_PHY_RGS_CMD_RGS_ARCS1_OFFSET_FLAG             Fld(1, 14) //[14:14]
#define MISC_PHY_RGS_CMD_RGS_ARCS2_OFFSET_FLAG             Fld(1, 15) //[15:15]
#define MISC_PHY_RGS_CMD_RGS_RX_ARCLK_RDY_EYE              Fld(1, 16) //[16:16]
#define MISC_PHY_RGS_CMD_RGS_RIMPCALOUT                    Fld(1, 24) //[24:24]

#define DDRPHY_MISC_PHY_RGS_STBEN_B0                   (DDRPHY_NAO_BASE_ADDRESS + 0x00198)
#define MISC_PHY_RGS_STBEN_B0_AD_RX_ARDQ0_STBEN_B0         Fld(8, 0) //[7:0]
#define MISC_PHY_RGS_STBEN_B0_AD_RX_ARDQ4_STBEN_B0         Fld(8, 8) //[15:8]
#define MISC_PHY_RGS_STBEN_B0_AD_RX_ARDQS0_STBEN_LEAD_B0   Fld(1, 16) //[16:16]
#define MISC_PHY_RGS_STBEN_B0_AD_RX_ARDQS0_STBEN_LAG_B0    Fld(1, 17) //[17:17]

#define DDRPHY_MISC_PHY_RGS_STBEN_B1                   (DDRPHY_NAO_BASE_ADDRESS + 0x0019C)
#define MISC_PHY_RGS_STBEN_B1_AD_RX_ARDQ0_STBEN_B1         Fld(8, 0) //[7:0]
#define MISC_PHY_RGS_STBEN_B1_AD_RX_ARDQ4_STBEN_B1         Fld(8, 8) //[15:8]
#define MISC_PHY_RGS_STBEN_B1_AD_RX_ARDQS0_STBEN_LEAD_B1   Fld(1, 16) //[16:16]
#define MISC_PHY_RGS_STBEN_B1_AD_RX_ARDQS0_STBEN_LAG_B1    Fld(1, 17) //[17:17]

#define DDRPHY_MISC_PHY_RGS_STBEN_CMD                  (DDRPHY_NAO_BASE_ADDRESS + 0x001A0)
#define MISC_PHY_RGS_STBEN_CMD_AD_RX_ARCA0_STBEN           Fld(8, 0) //[7:0]
#define MISC_PHY_RGS_STBEN_CMD_AD_RX_ARCA4_STBEN           Fld(8, 8) //[15:8]
#define MISC_PHY_RGS_STBEN_CMD_AD_RX_ARCLK_STBEN_LEAD      Fld(1, 16) //[16:16]
#define MISC_PHY_RGS_STBEN_CMD_AD_RX_ARCLK_STBEN_LAG       Fld(1, 17) //[17:17]

#define DDRPHY_GATING_CTRL_STATUS0                     (DDRPHY_NAO_BASE_ADDRESS + 0x00200)
#define GATING_CTRL_STATUS0_STB_GATING_ERROR               Fld(1, 0) //[0:0]
#define GATING_CTRL_STATUS0_STB_CNT_SHUF_ST_ERR_FLAG_RK0_B0 Fld(1, 1) //[1:1]
#define GATING_CTRL_STATUS0_DQSIEN_PICG_HEAD_ERR_FLAG_RK0_B0 Fld(1, 2) //[2:2]
#define GATING_CTRL_STATUS0_PI_OVERFLOW_B0                 Fld(1, 3) //[3:3]
#define GATING_CTRL_STATUS0_GATING_ERR_INF_STATUS          Fld(4, 4) //[7:4]
#define GATING_CTRL_STATUS0_STBUPD_STP                     Fld(1, 8) //[8:8]

#define DDRPHY_GATING_CTRL_STATUS1                     (DDRPHY_NAO_BASE_ADDRESS + 0x00204)
#define GATING_CTRL_STATUS1_DQS_F_COUNTER_B0               Fld(8, 0) //[7:0]
#define GATING_CTRL_STATUS1_DQS_R_COUNTER_B0               Fld(8, 16) //[23:16]

#define DDRPHY_GATING_CTRL_STATUS2                     (DDRPHY_NAO_BASE_ADDRESS + 0x00208)
#define GATING_CTRL_STATUS2_DQS_STBCAL_DEC_COUNTER_B0      Fld(16, 0) //[15:0]
#define GATING_CTRL_STATUS2_DQS_STBCAL_INC_COUNTER_B0      Fld(16, 16) //[31:16]

#define DDRPHY_GATING_CTRL_STATUS3                     (DDRPHY_NAO_BASE_ADDRESS + 0x0020C)
#define GATING_CTRL_STATUS3_DQS_STBCAL_CNT_RK0_B0          Fld(17, 0) //[16:0]

#define DDRPHY_GATING_TRACKING_STATUS0                 (DDRPHY_NAO_BASE_ADDRESS + 0x00210)
#define GATING_TRACKING_STATUS0_DQSIEN_PI_DELAY_RK0_B0     Fld(7, 0) //[6:0]
#define GATING_TRACKING_STATUS0_DQSIEN_UI_DELAY_RK0_B0     Fld(8, 16) //[23:16]

#define DDRPHY_PSRAM_TOGGLE_CNT                        (DDRPHY_NAO_BASE_ADDRESS + 0x0021C)
#define PSRAM_TOGGLE_CNT_TOGGLE_CNT                        Fld(32, 0) //[31:0]

#define DDRPHY_PSRAM_DQS0_ERR_CNT                      (DDRPHY_NAO_BASE_ADDRESS + 0x00220)
#define PSRAM_DQS0_ERR_CNT_DQS0_ERR_CNT                    Fld(32, 0) //[31:0]
#endif /* #ifndef __DDRPHY_NAO_REGS_H__ */
