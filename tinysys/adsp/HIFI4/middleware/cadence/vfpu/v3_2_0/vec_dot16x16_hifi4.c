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
#include "NatureDSP_Signal.h"
#include "NatureDSP_types.h"
#include "common.h"

/*===========================================================================
  Vector matematics:
  vec_dot              Vector Dot Product
===========================================================================*/

/*-------------------------------------------------------------------------
  Vector Dot product
  Two versions of routines are available: regular versions (without suffix _fast)
  that work with arbitrary arguments, faster versions 
  (with suffix _fast) apply some restrictions.

  Precision: 
  64x32  64x32-bit data, 64-bit output (fractional multiply Q63xQ31->Q63)
  64x64  64x64-bit data, 64-bit output (fractional multiply Q63xQ63->Q63)
  64x64i 64x64-bit data, 64-bit output (low 64 bit of integer multiply)
  32x32  32x32-bit data, 64-bit output
  32x16  32x16-bit data, 64-bit output
  16x16  16x16-bit data, 64-bit output for regular version and 32-bit for 
                        fast version
  f      single precision floating point

  Input:
  x[N]  input data, Q31 or floating point
  y[N]  input data, Q31, Q15, or floating point
  N	length of vectors
  Returns: dot product of all data pairs, Q31 or Q63

  Restrictions:
  Regular versions (without suffix _fast):
  None
  Faster versions (with suffix _fast):
  x,y - aligned on 8-byte boundary
  N   - multiple of 4

  vec_dot16x16_fast utilizes 32-bit saturating accumulator, so, input data 
  should be scaled properly to avoid erroneous results
-------------------------------------------------------------------------*/
int64_t vec_dot16x16 (const int16_t * restrict x,const int16_t * restrict y,int N)
{
    ae_valign      x_align, y_align;
    ae_int16x4 x0,y0;
    ae_int64 z0;
    xtbool4 bmask;
    int n;
    const ae_int16x4 * restrict px = (const ae_int16x4 *)x;
    const ae_int16x4 * restrict py = (const ae_int16x4 *)y;
    if(N<=0) return 0;
    z0 = AE_ZERO64();
    x_align = AE_LA64_PP(px);
    y_align = AE_LA64_PP(py);
    for (n=0; n<(N-3); n+=4)
    {
        AE_LA16X4_IP(x0,x_align,px);
        AE_LA16X4_IP(y0,y_align,py);
        AE_MULAAAAQ16(z0,x0,y0);
    }
    // tail
    bmask=AE_MOVBA4(0xF0>>(N&3));
    AE_LA16X4_IP(x0,x_align,px);
    AE_LA16X4_IP(y0,y_align,py);
    AE_MOVF16X4(y0,AE_ZERO16(),bmask);
    AE_MULAAAAQ16(z0,x0,y0);

    z0=AE_SLAI64S(z0,1);
    return z0;
}
