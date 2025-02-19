#include "dramc_common.h"
#include "dramc_pi_api.h"
#include "x_hal_io.h"

void Psram_FT(DRAMC_CTX_T *p)
{
    //need get log in real chip
    //DUMP_INIT_RG_LOG_TO_DE 1
}

void Psram_FT_Test_Chip(DRAMC_CTX_T *p)
{
#if 0
    //vPsramcInit_PreSettings Start
    ucDramRegWrite_1(0x701b8, 0x80a0a);
    ucDramRegWrite_1(0x702a0, 0x1080);
    ucDramRegWrite_1(0x702a0, 0x3080);
    ucDramRegWrite_1(0x702a0, 0x80003080);
    //vPsramcInit_PreSettings End
    //PSramcInitSetting Start
    ucDramRegWrite_1(0x70308, 0x50003);
    ucDramRegWrite_1(0x70284, 0x101);
    ucDramRegWrite_1(0x70284, 0x101);
    ucDramRegWrite_1(0x70314, 0x38010000);
    ucDramRegWrite_1(0x7027c, 0x0);
    ucDramRegWrite_1(0x70274, 0x0);
    ucDramRegWrite_1(0x70310, 0x333f3f00);
    ucDramRegWrite_1(0x70294, 0x11400000);
    ucDramRegWrite_1(0x70d84, 0x1f);
    ucDramRegWrite_1(0x70c1c, 0x13000010);
    ucDramRegWrite_1(0x70c20, 0x200000);
    ucDramRegWrite_1(0x70c24, 0x165ee3);
    ucDramRegWrite_1(0x70330, 0x0);
    ucDramRegWrite_1(0x70d90, 0xe57800ff);
    ucDramRegWrite_1(0x70d98, 0xe57800ff);
    ucDramRegWrite_1(0x70db8, 0x0);
    ucDramRegWrite_1(0x70dd0, 0x0);
    ucDramRegWrite_1(0x701a0, 0x0);
    ucDramRegWrite_1(0x700a0, 0x0);
    ucDramRegWrite_1(0x700bc, 0x0);
    ucDramRegWrite_1(0x70338, 0x10000000);
    ucDramRegWrite_1(0x701c0, 0x0);
    ucDramRegWrite_1(0x705f0, 0x10000022);
    ucDramRegWrite_1(0x705f0, 0x10000222);
    ucDramRegWrite_1(0x70608, 0x20000000);
    ucDramRegWrite_1(0x70688, 0x20000000);
    ucDramRegWrite_1(0x70c14, 0x30000e);
    ucDramRegWrite_1(0x70604, 0x20002);
    ucDramRegWrite_1(0x70684, 0x20002);
    ucDramRegWrite_1(0x70608, 0xb0800000);
    ucDramRegWrite_1(0x70688, 0x90800000);
    ucDramRegWrite_1(0x705f0, 0x222);
    ucDramRegWrite_1(0x70338, 0x10000001);
    ucDramRegWrite_1(0x700bc, 0x1);
    ucDramRegWrite_1(0x700a8, 0x120a1a12);
    ucDramRegWrite_1(0x700ac, 0x10e10);
    ucDramRegWrite_1(0x700b0, 0x10c10c0);
    ucDramRegWrite_1(0x700ac, 0x2010e00);
    ucDramRegWrite_1(0x701a4, 0x8);
    ucDramRegWrite_1(0x701b0, 0x20000);
    ucDramRegWrite_1(0x70008, 0x11);
    ucDramRegWrite_1(0x70264, 0x8b8b0000);
    ucDramRegWrite_1(0x7026c, 0x80000000);
    ucDramRegWrite_1(0x70df0, 0xd8d0000);
    ucDramRegWrite_1(0x70008, 0x10);
    ucDramRegWrite_1(0x70264, 0x8b8b0000);
    ucDramRegWrite_1(0x700a4, 0x8);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70da0, 0x0);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70da4, 0x200);
    ucDramRegWrite_1(0x70dac, 0x100);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70da8, 0x40000);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7000c, 0xc0000);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70d80, 0x3);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x700a4, 0x40e);
    ucDramRegWrite_1(0x70d34, 0x666009);
    ucDramRegWrite_1(0x70c34, 0x698618);
    ucDramRegWrite_1(0x70d14, 0x0);
    ucDramRegWrite_1(0x70d00, 0x104010);
    ucDramRegWrite_1(0x70d18, 0xc0);
    ucDramRegWrite_1(0x70270, 0x50909);
    mcDELAY_MS(9);
    ucDramRegWrite_1(0x70c38, 0x4);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70004, 0x0);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70d90, 0xe57820ff);
    ucDramRegWrite_1(0x70d98, 0xe57820ff);
    ucDramRegWrite_1(0x70d18, 0x2603c0);
    ucDramRegWrite_1(0x70d38, 0x4e100);
    ucDramRegWrite_1(0x70c18, 0x260340);
    ucDramRegWrite_1(0x70c38, 0x80022404);
    ucDramRegWrite_1(0x70d90, 0xe5780000);
    ucDramRegWrite_1(0x70d98, 0xe5780000);
    ucDramRegWrite_1(0x70270, 0x50909);
    ucDramRegWrite_1(0x70308, 0x50003);
    ucDramRegWrite_1(0x70d00, 0x144010);
    ucDramRegWrite_1(0x70d34, 0x698619);
    ucDramRegWrite_1(0x70d38, 0x4e104);
    ucDramRegWrite_1(0x70d18, 0x2603c0);
    ucDramRegWrite_1(0x700b4, 0x55);
    ucDramRegWrite_1(0x701b4, 0x55);
    ucDramRegWrite_1(0x701a0, 0x200000);
    ucDramRegWrite_1(0x70284, 0x101);
    ucDramRegWrite_1(0x70284, 0x101);
    ucDramRegWrite_1(0x70c18, 0x80260340);
    ucDramRegWrite_1(0x7000c, 0xc0000);
    ucDramRegWrite_1(0x70000, 0x0);
    ucDramRegWrite_1(0x70004, 0x0);
    ucDramRegWrite_1(0x70088, 0x0);
    ucDramRegWrite_1(0x7032c, 0x880aec00);
    ucDramRegWrite_1(0x70088, 0x880aec00);
    ucDramRegWrite_1(0x70188, 0x8000);
    ucDramRegWrite_1(0x70080, 0x0);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70da0, 0x0);
    ucDramRegWrite_1(0x70da8, 0x0);
    ucDramRegWrite_1(0x70d94, 0x50000002);
    ucDramRegWrite_1(0x70d9c, 0x50000002);
    ucDramRegWrite_1(0x70180, 0x2);
    ucDramRegWrite_1(0x70080, 0x2);
    ucDramRegWrite_1(0x70184, 0x0);
    ucDramRegWrite_1(0x70084, 0x0);
    ucDramRegWrite_1(0x70c18, 0xb2a60340);
    ucDramRegWrite_1(0x70000, 0x80000000);
    ucDramRegWrite_1(0x70004, 0x80000000);
    mcDELAY_MS(100);
    ucDramRegWrite_1(0x70c18, 0xb6a60340);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70080, 0xa);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7000c, 0x4d0000);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7018c, 0x8000);
    ucDramRegWrite_1(0x7008c, 0xae800);
    ucDramRegWrite_1(0x7032c, 0x800);
    ucDramRegWrite_1(0x70088, 0x800);
    ucDramRegWrite_1(0x7032c, 0x0);
    ucDramRegWrite_1(0x70088, 0x0);
    mcDELAY_MS(2);
    ucDramRegWrite_1(0x70284, 0x111);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70088, 0x1);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x701a0, 0x0);
    ucDramRegWrite_1(0x700b4, 0x40);
    ucDramRegWrite_1(0x701b4, 0x40);
    ucDramRegWrite_1(0x700bc, 0x11);
    ucDramRegWrite_1(0x70338, 0x10000011);
    ucDramRegWrite_1(0x700bc, 0x100011);
    ucDramRegWrite_1(0x70338, 0x10100011);
    ucDramRegWrite_1(0x701c0, 0x0);
    ucDramRegWrite_1(0x70314, 0x190d020f);
    ucDramRegWrite_1(0x7029c, 0x190d0200);
    ucDramRegWrite_1(0x705f0, 0x1000222);
    ucDramRegWrite_1(0x706f0, 0x0);
    ucDramRegWrite_1(0x701b4, 0x0);
    ucDramRegWrite_1(0x701b4, 0x0);
    ucDramRegWrite_1(0x700b4, 0x0);
    ucDramRegWrite_1(0x70c1c, 0x13008010);
    ucDramRegWrite_1(0x70330, 0x8000);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x700bc, 0x100031);
    ucDramRegWrite_1(0x70338, 0x10100031);
    ucDramRegWrite_1(0x700b0, 0x10c50c0);
    ucDramRegWrite_1(0x70314, 0x190d0200);
    ucDramRegWrite_1(0x70330, 0xb000);
    ucDramRegWrite_1(0x70330, 0x3010);
    ucDramRegWrite_1(0x70d00, 0x144010);
    ucDramRegWrite_1(0x70608, 0x20000000);
    ucDramRegWrite_1(0x70324, 0x0);
    ucDramRegWrite_1(0x701c0, 0x0);
    ucDramRegWrite_1(0x700a4, 0x4ae);
    ucDramRegWrite_1(0x700a4, 0x4ac);
    ucDramRegWrite_1(0x700ac, 0x2010e00);
    ucDramRegWrite_1(0x701ac, 0x808);
    ucDramRegWrite_1(0x701b0, 0x80000);
    ucDramRegWrite_1(0x70268, 0x40);
    ucDramRegWrite_1(0x700b0, 0x10c50c0);
    ucDramRegWrite_1(0x701b0, 0x80000);
    ucDramRegWrite_1(0x700b0, 0x10c50c1);
    ucDramRegWrite_1(0x701b0, 0x80001);
    ucDramRegWrite_1(0x701b0, 0x80041);
    ucDramRegWrite_1(0x700b0, 0x10c50c1);
    ucDramRegWrite_1(0x701b0, 0x80041);
    ucDramRegWrite_1(0x700b0, 0x10c50e1);
    ucDramRegWrite_1(0x701b0, 0x80041);
    ucDramRegWrite_1(0x70c14, 0x30000b);
    ucDramRegWrite_1(0x700ac, 0x2010b00);
    ucDramRegWrite_1(0x700b8, 0x0);
    ucDramRegWrite_1(0x701bc, 0x18000);
    ucDramRegWrite_1(0x700b0, 0x10c90e1);
    ucDramRegWrite_1(0x70338, 0x10100031);
    ucDramRegWrite_1(0x700bc, 0x100031);
    ucDramRegWrite_1(0x70338, 0x10100020);
    ucDramRegWrite_1(0x700bc, 0x100020);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x700bc, 0x100031);
    ucDramRegWrite_1(0x70338, 0x10100031);
    ucDramRegWrite_1(0x701b8, 0x80a0a);
    ucDramRegWrite_1(0x701b8, 0x80a0a);
    ucDramRegWrite_1(0x70c14, 0x30000b);
    ucDramRegWrite_1(0x70d14, 0x0);
    ucDramRegWrite_1(0x70c18, 0xb6a60340);
    ucDramRegWrite_1(0x70d18, 0x2603c0);
    ucDramRegWrite_1(0x70334, 0xffc00000);
    ucDramRegWrite_1(0x70328, 0xffc07fff);
    ucDramRegWrite_1(0x7031c, 0x11351135);
    ucDramRegWrite_1(0x702a8, 0x11351135);
    ucDramRegWrite_1(0x70320, 0x800);
    ucDramRegWrite_1(0x70330, 0x10);
    ucDramRegWrite_1(0x70440, 0x0);
    ucDramRegWrite_1(0x70c34, 0x698619);
    ucDramRegWrite_1(0x70184, 0x200000);
    ucDramRegWrite_1(0x700b0, 0x10c90e1);
    ucDramRegWrite_1(0x701b0, 0x80041);
    ucDramRegWrite_1(0x70330, 0x13000010);
    ucDramRegWrite_1(0x70338, 0x10100431);
    ucDramRegWrite_1(0x700bc, 0x100431);
    ucDramRegWrite_1(0x701c0, 0x0);
    ucDramRegWrite_1(0x70334, 0xffc00000);
    ucDramRegWrite_1(0x70c38, 0x80002004);
    ucDramRegWrite_1(0x70284, 0x80000111);
    ucDramRegWrite_1(0x70084, 0x200000);
    ucDramRegWrite_1(0x70184, 0x200000);
    ucDramRegWrite_1(0x70314, 0x110d0200);
    ucDramRegWrite_1(0x705e8, 0x101);
    ucDramRegWrite_1(0x701d0, 0xa94011c0);
    ucDramRegWrite_1(0x70338, 0x101004b1);
    ucDramRegWrite_1(0x700bc, 0x100431);
    ucDramRegWrite_1(0x702a0, 0x8100308c);
    ucDramRegWrite_1(0x70318, 0x100008c);
    ucDramRegWrite_1(0x70278, 0x0);
    ucDramRegWrite_1(0x70260, 0x20000000);
    ucDramRegWrite_1(0x70260, 0x20000000);
    ucDramRegWrite_1(0x70ec4, 0x10000000);
    ucDramRegWrite_1(0x70ec4, 0x10001000);
    ucDramRegWrite_1(0x70e1c, 0x1000);
    ucDramRegWrite_1(0x70e1c, 0x101000);
    ucDramRegWrite_1(0x40718, 0x2);
    ucDramRegWrite_1(0x40720, 0x22222222);
    ucDramRegWrite_1(0x40730, 0x11111111);
    ucDramRegWrite_1(0x40600, 0x110001);
    ucDramRegWrite_1(0x40608, 0x610001);
    ucDramRegWrite_1(0x4077c, 0x20004);
    ucDramRegWrite_1(0x40600, 0x130004);
    ucDramRegWrite_1(0x40608, 0x630003);
    ucDramRegWrite_1(0x70e1c, 0x101000);
    ucDramRegWrite_1(0x40604, 0x30004);
    ucDramRegWrite_1(0x4060c, 0x30003);
    ucDramRegWrite_1(0x70e1c, 0x101000);
    ucDramRegWrite_1(0x40780, 0x30003);
    ucDramRegWrite_1(0x70404, 0x71111);
    ucDramRegWrite_1(0x70418, 0x1a0010);
    ucDramRegWrite_1(0x70500, 0xf0f10);
    ucDramRegWrite_1(0x70e08, 0x0);
    ucDramRegWrite_1(0x70e0c, 0x0);
    ucDramRegWrite_1(0x70e10, 0x0);
    ucDramRegWrite_1(0x70e14, 0x0);
    ucDramRegWrite_1(0x70e18, 0xb0b0000);
    ucDramRegWrite_1(0x401fc, 0xc0420006);
    ucDramRegWrite_1(0x40710, 0x480);
    ucDramRegWrite_1(0x40170, 0x80);
    ucDramRegWrite_1(0x4073c, 0x3f);
    ucDramRegWrite_1(0x4023c, 0x10);
    ucDramRegWrite_1(0x40244, 0x80000000);
    ucDramRegWrite_1(0x40260, 0x8040000);
    ucDramRegWrite_1(0x40260, 0x8040000);
    ucDramRegWrite_1(0x40264, 0x25000000);
    ucDramRegWrite_1(0x40268, 0x30000060);
    ucDramRegWrite_1(0x40700, 0x3);
    ucDramRegWrite_1(0x40710, 0x20000480);
    ucDramRegWrite_1(0x4078c, 0x3f);
    ucDramRegWrite_1(0x40110, 0x1018310);
    ucDramRegWrite_1(0x40114, 0x1a0068);
    ucDramRegWrite_1(0x40740, 0x11a060c);
    ucDramRegWrite_1(0x40760, 0xc);
    ucDramRegWrite_1(0x40744, 0x1a1010);
    ucDramRegWrite_1(0x40748, 0xd110d);
    ucDramRegWrite_1(0x40764, 0xfc);
    ucDramRegWrite_1(0x4074c, 0x1a);
    ucDramRegWrite_1(0x40750, 0x820105);
    ucDramRegWrite_1(0x40768, 0x2);
    ucDramRegWrite_1(0x40754, 0x19);
    ucDramRegWrite_1(0x402f0, 0xca0065);
    ucDramRegWrite_1(0x4012c, 0x500);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70278, 0x0);
    ucDramRegWrite_1(0x7027c, 0x0);
    ucDramRegWrite_1(0x70274, 0x0);
    ucDramRegWrite_1(0x70d2c, 0x2b00);
    ucDramRegWrite_1(0x7026c, 0x80120000);
    ucDramRegWrite_1(0x70268, 0x50);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70268, 0x51);
    ucDramRegWrite_1(0x7026c, 0x80120000);
    ucDramRegWrite_1(0x70d2c, 0x2b01);
    ucDramRegWrite_1(0x7026c, 0x80120000);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120001);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120002);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120003);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120004);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120005);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120006);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70d2c, 0x2b01);
    ucDramRegWrite_1(0x70268, 0x50);
    ucDramRegWrite_1(0x7026c, 0x80120006);
    ucDramRegWrite_1(0x70d2c, 0x2b01);
    ucDramRegWrite_1(0x7026c, 0x80120006);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120106);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120206);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120306);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120406);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120506);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x7026c, 0x80120606);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70d2c, 0x2b01);
    ucDramRegWrite_1(0x70c04, 0x6000c0c);
    ucDramRegWrite_1(0x70c08, 0x6000c0c);
    ucDramRegWrite_1(0x70268, 0x40);
    ucDramRegWrite_1(0x7026c, 0x80000606);
    ucDramRegWrite_1(0x70d2c, 0x0);
    ucDramRegWrite_1(0x70314, 0x190d0200);
    ucDramRegWrite_1(0x70084, 0x200000);
    ucDramRegWrite_1(0x70084, 0x200000);
    ucDramRegWrite_1(0x70c34, 0x698619);
    ucDramRegWrite_1(0x70284, 0x801bff11);
    ucDramRegWrite_1(0x401fc, 0x42006a);
    ucDramRegWrite_1(0x4023c, 0x11);
    ucDramRegWrite_1(0x40244, 0x80000000);
    ucDramRegWrite_1(0x40260, 0x8040000);
    ucDramRegWrite_1(0x40168, 0x580701);
    ucDramRegWrite_1(0x40190, 0x4);
    ucDramRegWrite_1(0x40204, 0x40);
    ucDramRegWrite_1(0x40108, 0x2000980);
    ucDramRegWrite_1(0x7028c, 0x8060036e);
    ucDramRegWrite_1(0x7028c, 0x8060036f);
    ucDramRegWrite_1(0x7028c, 0x8060036e);
    ucDramRegWrite_1(0x70324, 0x0);
    ucDramRegWrite_1(0x70330, 0x13000010);
    ucDramRegWrite_1(0x70334, 0xffc00000);
    ucDramRegWrite_1(0x70450, 0x0);
    ucDramRegWrite_1(0x70454, 0x0);
    ucDramRegWrite_1(0x70298, 0x0);
    ucDramRegWrite_1(0x40204, 0x40);
    mcDELAY_MS(3);
    ucDramRegWrite_1(0x40124, 0x1);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x40124, 0x0);
    mcDELAY_MS(10);
    ucDramRegWrite_1(0x40204, 0x0);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x4012c, 0x500);
    ucDramRegWrite_1(0x4012c, 0x500);
    ucDramRegWrite_1(0x40124, 0x4000);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x40124, 0x0);
    ucDramRegWrite_1(0x40128, 0x0);
    ucDramRegWrite_1(0x40128, 0x13);
    ucDramRegWrite_1(0x40124, 0x800);
    ucDramRegWrite_1(0x40124, 0x0);
    ucDramRegWrite_1(0x40128, 0x213);
    ucDramRegWrite_1(0x40128, 0x220);
    ucDramRegWrite_1(0x40124, 0x800);
    ucDramRegWrite_1(0x40124, 0x0);
    ucDramRegWrite_1(0x40264, 0x5000000);
    //PSramcInitSetting End
    //vApplyPsramConfigBeforeCalibration Start
    ucDramRegWrite_1(0x70ea0, 0x0);
    ucDramRegWrite_1(0x70e00, 0x0);
    ucDramRegWrite_1(0x70e04, 0x0);
    ucDramRegWrite_1(0x70608, 0x20000000);
    ucDramRegWrite_1(0x70608, 0x20000000);
    ucDramRegWrite_1(0x70608, 0x20000000);
    ucDramRegWrite_1(0x70708, 0x0);
    ucDramRegWrite_1(0x70318, 0x100008c);
    ucDramRegWrite_1(0x702a0, 0x8100308c);
    ucDramRegWrite_1(0x700b0, 0x10c90e1);
    ucDramRegWrite_1(0x701b0, 0x80041);
    ucDramRegWrite_1(0x70c14, 0x40000b);
    ucDramRegWrite_1(0x70330, 0x13000010);
    ucDramRegWrite_1(0x70c1c, 0x13008010);
    ucDramRegWrite_1(0x40110, 0x3018310);
    //vApplyPsramConfigBeforeCalibration End
    //PSramcRxdqsGatingCal Start
    ucDramRegWrite_1(0x70418, 0x190014);
    ucDramRegWrite_1(0x70418, 0x19000c);
    ucDramRegWrite_1(0x70420, 0xf);
    ucDramRegWrite_1(0x40240, 0x4000001d);
    ucDramRegWrite_1(0x70318, 0x100008e);
    ucDramRegWrite_1(0x70338, 0x101004a0);
    ucDramRegWrite_1(0x702a0, 0x8100308e);
    ucDramRegWrite_1(0x700bc, 0x100420);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70338, 0x101004b1);
    ucDramRegWrite_1(0x702a0, 0x8100308c);
    ucDramRegWrite_1(0x70318, 0x100008c);
    ucDramRegWrite_1(0x700bc, 0x100431);
    ucDramRegWrite_1(0x40240, 0x4000001c);
    //PSramcRxdqsGatingCal End
    //PSramcRxWindowPerbitCal Start
    ucDramRegWrite_1(0x70e18, 0xf0f);
    ucDramRegWrite_1(0x70e08, 0x4040606);
    ucDramRegWrite_1(0x70e0c, 0x5050505);
    ucDramRegWrite_1(0x70e10, 0x5050606);
    ucDramRegWrite_1(0x70e14, 0x5050606);
    ucDramRegWrite_1(0x40240, 0x4000001d);
    ucDramRegWrite_1(0x70318, 0x100008e);
    ucDramRegWrite_1(0x70338, 0x101004a0);
    ucDramRegWrite_1(0x702a0, 0x8100308e);
    ucDramRegWrite_1(0x700bc, 0x100420);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70338, 0x101004b1);
    ucDramRegWrite_1(0x702a0, 0x8100308c);
    ucDramRegWrite_1(0x70318, 0x100008c);
    ucDramRegWrite_1(0x700bc, 0x100431);
    ucDramRegWrite_1(0x40240, 0x4000001c);
    //PSramcRxWindowPerbitCal End
    //PSramcTxWindowPerbitCal Start
    ucDramRegWrite_1(0x40600, 0x130004);
    ucDramRegWrite_1(0x40608, 0x630003);
    ucDramRegWrite_1(0x70e1c, 0x1f0e00);
    ucDramRegWrite_1(0x40604, 0x30004);
    ucDramRegWrite_1(0x4060c, 0x30003);
    ucDramRegWrite_1(0x70e1c, 0xe0e00);
    ucDramRegWrite_1(0x40720, 0x22222222);
    ucDramRegWrite_1(0x40730, 0x11111111);
    ucDramRegWrite_1(0x40600, 0x130004);
    ucDramRegWrite_1(0x40608, 0x630003);
    //PSramcTxWindowPerbitCal End
    //PSramcTxWindowPerbitCal Start
    ucDramRegWrite_1(0x40600, 0x130004);
    ucDramRegWrite_1(0x40608, 0x630003);
    ucDramRegWrite_1(0x70e1c, 0xe0e00);
    ucDramRegWrite_1(0x40604, 0x30004);
    ucDramRegWrite_1(0x4060c, 0x30003);
    ucDramRegWrite_1(0x70e1c, 0xe0e00);
    ucDramRegWrite_1(0x40720, 0x22222222);
    ucDramRegWrite_1(0x40730, 0x11111111);
    ucDramRegWrite_1(0x40600, 0x130004);
    ucDramRegWrite_1(0x40608, 0x630003);
    //PSramcTxWindowPerbitCal End
    //PSramcRxdatlatScan Start
    ucDramRegWrite_1(0x70500, 0x101011);
    ucDramRegWrite_1(0x4078c, 0x3f);
    ucDramRegWrite_1(0x40240, 0x4000001d);
    ucDramRegWrite_1(0x70318, 0x100008e);
    ucDramRegWrite_1(0x70338, 0x101004a0);
    ucDramRegWrite_1(0x702a0, 0x8100308e);
    ucDramRegWrite_1(0x700bc, 0x100420);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70338, 0x101004b1);
    ucDramRegWrite_1(0x702a0, 0x8100308c);
    ucDramRegWrite_1(0x70318, 0x100008c);
    ucDramRegWrite_1(0x700bc, 0x100431);
    ucDramRegWrite_1(0x40240, 0x4000001c);
    ucDramRegWrite_1(0x70414, 0x11030);
    mcDELAY_MS(1);
    ucDramRegWrite_1(0x70414, 0x11010);
    //PSramcRxdatlatScan End
    //vApplyPsramConfigAfterCalibration Start
    ucDramRegWrite_1(0x70310, 0x11400000);
    ucDramRegWrite_1(0x70334, 0xffe00000);
    ucDramRegWrite_1(0x700b8, 0x8000);
    ucDramRegWrite_1(0x701b0, 0x80001);
    ucDramRegWrite_1(0x700b0, 0x10c90e0);
    ucDramRegWrite_1(0x701b0, 0x80000);
    ucDramRegWrite_1(0x700b0, 0x10c90e0);
    ucDramRegWrite_1(0x701b0, 0x80000);
    ucDramRegWrite_1(0x70284, 0x801bff10);
    ucDramRegWrite_1(0x70314, 0x190d0200);
    ucDramRegWrite_1(0x70318, 0x100008c);
    ucDramRegWrite_1(0x70414, 0x10000);
    ucDramRegWrite_1(0x40264, 0x5000000);
    ucDramRegWrite_1(0x70420, 0x10040f);
    ucDramRegWrite_1(0x70424, 0x22);
    ucDramRegWrite_1(0x7042c, 0x120011);
    ucDramRegWrite_1(0x70430, 0x20001);
    ucDramRegWrite_1(0x40108, 0x12000900);
    ucDramRegWrite_1(0x40110, 0x3018310);
    //vApplyPsramConfigAfterCalibration Start
    mcDELAY_MS(1);
#endif /* #if 0 */
}

