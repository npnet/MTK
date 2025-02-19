/*
 *    Mediatek HiFi 4 Redistribution Version  <0.0.1>
 */

/* ------------------------------------------------------------------------ */
/* Copyright (c) 2016 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED.  */
/* These coded instructions, statements, and computer programs (�Cadence    */
/* Libraries�) are the copyrighted works of Cadence Design Systems Inc.	    */
/* Cadence IP is licensed for use with Cadence processor cores only and     */
/* must not be used for any other processors and platforms. Your use of the */
/* Cadence Libraries is subject to the terms of the license agreement you   */
/* have entered into with Cadence Design Systems, or a sublicense granted   */
/* to you by a direct Cadence licensee.                                     */
/* ------------------------------------------------------------------------ */
/*  IntegrIT, Ltd.   www.integrIT.com, info@integrIT.com                    */
/*                                                                          */
/* DSP Library                                                              */
/*                                                                          */
/* This library contains copyrighted materials, trade secrets and other     */
/* proprietary information of IntegrIT, Ltd. This software is licensed for  */
/* use with Cadence processor cores only and must not be used for any other */
/* processors and platforms. The license to use these sources was given to  */
/* Cadence, Inc. under Terms and Condition of a Software License Agreement  */
/* between Cadence, Inc. and IntegrIT, Ltd.                                 */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2015-2016 IntegrIT, Limited.                      */
/*                      All Rights Reserved.                                */
/* ------------------------------------------------------------------------ */
#include "sine_table32.h"
#include "recip_table.h"
#include "common.h"

/*===========================================================================
  Vector matematics:
  vec_tan             Tangent    
===========================================================================*/

/*-------------------------------------------------------------------------
  Tangent 
  Fixed point functions calculate tan(pi*x) for number written in Q31. 
  Floating point functions compute tan(x)
  
  Precision: 
  32x32  32-bit inputs, 32-bit outputs. Accuracy: (1.3e-4*y+1LSB)
                                        if abs(y)<=464873(14.19 in Q15) 
                                        or abs(x)<pi*0.4776
  f      floating point.                Accuracy: 2 ULP

  NOTE:
  1.  Scalar floating point function is compatible with standard ANSI C routines 
      and set errno and exception flags accordingly
  2.  Floating point functions limit the range of allowable input values: [-9099, 9099]
      Whenever the input value does not belong to this range, the result is set to NaN.

  Input:
  x[N]   input data,Q31 or floating point
  N      length of vectors
  Output:
  y[N]   result, Q16.15 or floating point

  Restriction:
  x,y - should not overlap

  Scalar versions:
  ----------------
  Return result, Q16.15 or floating point
-------------------------------------------------------------------------*/
void vec_tan32x32 ( 
              int32_t* restrict   z, 
              const int32_t *restrict x,
              int N)
{
    int           n, ind,y_nsa,indb,y_nsb;
        ae_int32x2 * restrict pz     = (      ae_int32x2 *)z;
    const ae_int32x2 * restrict px     = (const ae_int32x2 *)x;
    const ae_int32   * restrict ptbl1  = (const ae_int32   *)sine_table32;
    const ae_p16x2s * restrict ptbl2  = (const ae_p16x2s *)recip_table;
    ae_valign x_align, z_align;

    uint32_t    off,offb;
    ae_int32x2  vzw,vaw,vxw,vnw,vlw,vkw,vrw,vxa,vxb,vxc,vxd, vdel,vcos,vsin,vww,vtt;
    ae_int32x2  vdw,vcw,vsw;
    ae_f32x2    vsf,vdf,vxf,vbf,vcf,vzf;
    ae_f32x2    vyf;
    ae_int32x2  vyw, y1,vmw,www;
    xtbool2     sgya;
    ae_int32x2  vya,vyb,vy1,vy2,vyy;
    ae_int32x2  t1,t2;
    if(N<=0) return;
    x_align = AE_LA64_PP(px);
    z_align = AE_ZALIGN64();

    vzw = AE_MOVI(0);
    www = AE_MOVDA32X2(0xc91000, 0xc91000); //pi,Q22
    vww = AE_MOVDA32X2(0xFFFFFFFC, 0xFFFFFFFC);
    vnw = AE_MOVDA32X2(16384, 16384);
    vtt = AE_MOVDA32X2(0x400000, 0x400000); //1<<(22)
    vlw = AE_MOVDA32X2(0x200, 0x200); //1<<(9)/4
    vkw = AE_MOVDA32X2(0x7fc, 0x7fc); //1<<(9)-1
    vmw = AE_MOVDA32X2(0xE0000000, 0xE0000000); //-1<<(29)
    __Pragma("concurrent")
    for (n=0; n<(N>>1); ++n)
    {
        AE_LA32X2_IP(vxw, x_align, px); 
        /*Calculate sine and cosine*/   
        vaw = AE_ADD32S(vxw,vtt);

        vaw = AE_SRLI32(vaw,21);
        vaw = AE_AND32(vaw, vww);

        off = AE_MOVAD32_H(vaw);
        offb = AE_MOVAD32_L(vaw);
        /*Load sine */
        vxa = AE_L32_X(ptbl1, off); //sin
        vxb = AE_L32_X(ptbl1, offb); //sin b
        vsin = AE_SEL32_HH(vxa, vxb); //sin

        vdw = AE_SLAI32(vaw,21);
        vdw = AE_SUB32(vxw, vdw);
         
        vdel = AE_MULFP32X2RAS(vdw, www);//

        vaw = AE_ADD32S(vaw,vlw);
        vaw = AE_AND32(vaw, vkw);
        off = AE_MOVAD32_H(vaw);
        offb = AE_MOVAD32_L(vaw);
        /*Load cosine*/
        vxc = AE_L32_X(ptbl1, off); //cos vxa
        vxd = AE_L32_X(ptbl1, offb); //cos b
        vcos = AE_SEL32_HH(vxc, vxd); //cos

        vsf = (vsin);//sin
        vrw = AE_SLAI32S(vdel, 8); //
        vdel = AE_SLAI32S(vdel, 9); //dx,Q31

        vxf = (vcos);
        vcf = (vsin);
        AE_MULAFP32X2RAS(vcf, vcos, vrw);//sin+1/2*cos*dx
        AE_MULSFP32X2RAS(vxf, vsin, vrw);//cos - 1/2*sin*dx

        vzf = (vcos);//dx
        AE_MULAFP32X2RAS(vsf, vxf, vdel); //y = sin(x0)+cos(x0)dx-1/2*sin(x0)*dx^2,Q31
        AE_MULSFP32X2RAS(vzf, vcf, vdel); //y = cos(x0)-sin(x0)dx-1/2*cos(x0)*dx^2,Q31
        vsw = (vsf);//sin
        vcw = (vzf);//cos
        /*Calculate sine/cosine*/
        sgya = AE_LT32(vzf, vzw);
        vyb = AE_ABS32S(vzf);
        vya = AE_SEL32_HH(vyb,vyb);
        /*Normalize cosine*/
        y_nsa = AE_NSAZ32_L(vya);
        y_nsb = AE_NSAZ32_L(vyb);
        vya = AE_SLAA32S( vya, y_nsa );//y in 0.5..1 
        vyb = AE_SLAA32S( vyb, y_nsb );//y in 0.5..1 
        y_nsa =y_nsa-15;
        y_nsb =y_nsb-15;
        vyy = AE_SEL32_HL(vya,vyb);
        /*Calculate reciprocal cosine for the 16 high-order bits
        recip = f(y0)+f'(y0)*dy
        recip = (1/y0)-(1/y0^2)*(y-y0)
        */
        y1 = AE_SRAI32(vyy,16);//y,Q1.14
        y1 = AE_SUB32(y1,vnw);

        vdw = AE_SRAI32(y1,5);
        vdw = AE_AND32(vdw, vww);

        ind = AE_MOVAD32_H(vdw);
        indb = AE_MOVAD32_L(vdw);
        vy1 = AE_L16X2M_X(ptbl2, ind);//1
        vy2 = AE_L16X2M_X(ptbl2, indb);//2

        t2 = AE_SEL32_LL(vy1,vy2);//f'
        t1 = AE_SEL32_HH(vy1,vy2);//f

        vdw = AE_SLAI32S(vdw,5);
        vdw = AE_SUB32(y1,vdw);//dy
        vdw = AE_SLAI32S(vdw,9);
        vbf = AE_MULFP32X2RAS(t2,vdw);//(1/y^2)*dy
        vyw = AE_SLAI32(vbf, 8); //
        vy1 = AE_SUB32S(t1,vyw);

        /*refinement*/
        y1 = AE_SLAI32(vy1,7);  //Q30
        vzf = (vmw);
        AE_MULAFP32X2RAS(vzf,vyy,y1);//err=(1/y)*y-1,Q1.29

        t2 = AE_SLAI32S(vzf,2);
        vyf = AE_SLAI32(vy1,8);
        AE_MULSFP32X2RAS(vyf,vyf,t2);   //Q31 y0-err*y0 

        t2 = AE_MULFP32X2RAS(vyf,vsw);//(x/y)
        t1 = AE_NEG32(t2);
        AE_MOVT32X2(t2, t1, sgya);
        t1 = AE_SLAA32S(t2,y_nsa);
        t2 = AE_SLAA32S(t2,y_nsb);
        vrw = AE_SEL32_HL(t1,t2);
        AE_SA32X2_IP(vrw, z_align, pz);//put answer
    }
    AE_SA64POS_FP(z_align, pz);
    if (N&1)
    {
        AE_L32_IP(vxw,castxcc(ae_int32,px),0); 
        /*Calculate sine and cosine*/  
        vaw = AE_ADD32S(vxw,vtt);

        vaw = AE_SRLI32(vaw,21);
        vaw = AE_AND32(vaw, vww);

        off = AE_MOVAD32_H(vaw);
        /*Load sine */
        vsin = AE_L32_X(ptbl1, off); //sin

        vdw = AE_SLAI32(vaw,21);
        vdw = AE_SUB32(vxw, vdw);
         
        vdel = AE_MULFP32X2RAS(vdw, www);//

        vaw = AE_ADD32S(vaw,vlw);
        vaw = AE_AND32(vaw, vkw);
        off = AE_MOVAD32_H(vaw);
        /*Load cosine*/
        vcos = AE_L32_X(ptbl1, off); //cos vxa
        vsf = (vsin);//sin
        vrw = AE_SLAI32(vdel, 8); //
        vdf = (vrw);//1/2*dx
        vdel = AE_SLAI32(vdel, 9); //dx,Q31

        vxf = (vcos);
        vcf = (vsin);
        AE_MULAFP32X2RAS(vcf, vxf, vdf);//sin+1/2*cos*dx
        AE_MULSFP32X2RAS(vxf, vsf, vdf);//cos - 1/2*sin*dx

        vzf = (vcos);//dx
        AE_MULAFP32X2RAS(vsf, vxf, vdel); //y = sin(x0)+cos(x0)dx-1/2*sin(x0)*dx^2,Q31
        AE_MULSFP32X2RAS(vzf, vcf, vdel); //y = cos(x0)-sin(x0)dx-1/2*cos(x0)*dx^2,Q31
        vsw = (vsf);//sin
        vcw = (vzf);//cos
        /*Calculate sine/cosine*/
        sgya = AE_LT32(vcw, vzw);
        vya = AE_ABS32S(vcw);
        /*Normalize cosine*/
        y_nsa = AE_NSAZ32_L(vya);
        vyy = AE_SLAA32S( vya, y_nsa );//y in 0.5..1 
        y_nsa =y_nsa-15;
        /*Calculate reciprocal cosine for the 16 high-order bits
        recip = f(y0)+f'(y0)*dy
        recip = (1/y0)-(1/y0^2)*(y-y0)
        */
        y1 = AE_SRAI32(vyy,16);//y,Q1.14
        y1 = AE_SUB32(y1,vnw);

        vdw = AE_SRAI32(y1,5);
        vdw = AE_AND32(vdw, vww);

        ind = AE_MOVAD32_H(vdw);
        vy1 = AE_L16X2M_X(ptbl2, ind);//1
        t2 = AE_SEL32_LL(vy1,vy1);//f'
        t1 = AE_SEL32_HH(vy1,vy1);//f

        vdw = AE_SLAI32(vdw,5);
        vdw = AE_SUB32(y1,vdw);//dy
        vdw = AE_SLAI32(vdw,9);
        vbf = AE_MULFP32X2RAS(t2,vdw);//(1/y^2)*dy
        vyw = AE_SLAI32(vbf, 8); //
        vy1 = AE_SUB32S(t1,vyw);

        /*refinement*/
        y1  = AE_SLAI32(vy1,7);  //Q30
        vyf = AE_SLAI32(vy1,8);
        vzf = (vmw);
        AE_MULAFP32X2RAS(vzf,vyy,y1);//err=(1/y)*y-1,Q1.29
        t2 = AE_SLAI32(vzf,2);

        AE_MULSFP32X2RAS(vyf,vyf,t2);   //Q31 y0-err*y0 

        t2 = AE_MULFP32X2RAS(vyf,vsf);//(x/y)
        t1 = AE_NEG32(t2);
        AE_MOVT32X2(t2, t1, sgya);
        vrw = AE_SLAA32S(t2,y_nsa);
        AE_S32_L_I(vrw,(ae_int32 *)pz, 0);//put answer
    }
}
