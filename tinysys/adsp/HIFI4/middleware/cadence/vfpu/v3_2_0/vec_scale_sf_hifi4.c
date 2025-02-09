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
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,vec_scale_sf,(     float32_t * restrict y,
                    const float32_t * restrict x,
                    float32_t s, float32_t fmin, float32_t fmax,
                    int N))
#else

/*===========================================================================
Vector matematics:
vec_scale  Vector Scaling with Saturation
===========================================================================*/
/*-------------------------------------------------------------------------
  Vector Scaling with Saturation
  These routines make shift with saturation of data values in the vector 
  by given scale factor (degree of 2). 
  Functions vec_scale() make multiplication of Q31 vector to Q31 coefficient 
  which is not a power of 2 forming Q31 result.
  Two versions of routines are available: regular versions (vec_shift32x32, 
  vec_shift16x16, vec_shiftf, vec_scale16x16, vec_scale32x32, vec_scalef, 
  vec_scale_sf) work with arbitrary arguments, faster versions 
  (vec_shift32x32_fast, vec_shift16x16_fast, vec_scale16x16_fast, 
  vec_scale32x32_fast) apply some additional restricitons.

  For floating point:
  Function vec_shiftf makes shift without saturation of data values in the vector
  by given scale factor (degree of 2). 
  Functions vec_scalef() and vec_scale_sf() make multiplication of input vector
  to coefficient which is not a power of 2.
  Two versions of routines are available: 
    without saturation - vec_scalef;
    with saturation - vec_scale_sf; 

Precision:
32x32 32-bit input, 32-bit output
16x16 16-bit input, 16-bit output
f     single precision floating point

  Input:
  x[N]  input data , Q31,Q15 or floating point
  t     shift count. If positive, it shifts left with saturation, if
  negative it shifts right
  s     scale factor, Q31,Q15 or floating point
  N     length of vector
  fmin  minimum output value (only for vec_scale_sf)
  fmax  maximum output value (only for vec_scale_sf)

  Output:
  y[N]  output data, Q31, Q15 or floating point

  Restrictions:
  For regular versions (vec_shift32x32, vec_shift16x16, vec_shiftf, 
  vec_scale16x16, vec_scale32x32, vec_scalef, vec_scale_sf):
  x,y should not overlap
  t   should be in range -31...31 for fixed-point functions and -129...146 
      for floating point

  For vec_scale_sf - fmin<=fmax;

  For faster versions (vec_shift32x32_fast, vec_shift16x16_fast, 
  vec_scale16x16_fast, vec_scale32x32_fast):
  x,y should not overlap
  t should be in range -31...31 
  x,y - aligned on 8-byte boundary
  N   - multiple of 4 
-------------------------------------------------------------------------*/
#define sz_f32    (int)sizeof(float32_t)
void vec_scale_sf   (     float32_t * restrict y,
                    const float32_t * restrict x,
                    float32_t s, float32_t fmin, float32_t fmax,
                    int N)
{
  int n;

  xtfloatx2 vxf, vyf, vsf, f0, f1;
  xtfloat xf, yf;

  const xtfloatx2* restrict px = (const xtfloatx2 *)x;
  xtfloatx2* restrict py = (xtfloatx2 *)y;
  ae_valign x_align, y_align;
  NASSERT(x);
  NASSERT(y);
  NASSERT(fmin <= fmax);
  if (N <= 0) return ; 

  x_align = AE_LA64_PP(px);
  y_align = AE_ZALIGN64();

  vsf = XT_MOV_SX2(s);
  f0 = XT_MOV_SX2(fmin);
  f1 = XT_MOV_SX2(fmax);
  for (n = 0; n<(N >> 1); n ++)
  {
    XT_LASX2IP(vxf, x_align, px);
    vyf = XT_MUL_SX2(vxf, vsf);
    vyf = XT_MAX_SX2(vyf, f0);
    vyf = XT_MIN_SX2(vyf, f1);
    XT_SASX2IP(vyf, y_align, py);
  }
  AE_SA64POS_FP(y_align, py);
   
  if (N & 1)
  {
    xf = XT_LSI((const xtfloat *)px, 0);
    yf = XT_MUL_S(xf, s);
    yf = XT_MAX_S(yf, fmin);
    yf = XT_MIN_S(yf, fmax);
    XT_SSI(yf, (xtfloat *)py, 0);
  }
} /* vec_scale_sf() */
#endif
