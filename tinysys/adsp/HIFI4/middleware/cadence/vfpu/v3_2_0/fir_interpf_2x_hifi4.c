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
/*
    NatureDSP Signal Processing Library. FIR part
    Real interpolating FIR Filter
    C code optimized for HiFi4
    IntegrIT, 2006-2015
 */

/* Cross-platform data type definitions. */
#include "NatureDSP_types.h"
/* Common helper macros. */
#include "common.h"
/* Basic operations for the reference code. */
#include "baseop.h"
/* Filters and transformations */
#include "NatureDSP_Signal.h"
#include "fir_interpf_2x.h"
#if (XCHAL_HAVE_HIFI4_VFPU)

/*-------------------------------------------------------------------------
Real Interpolation FIR Filter
These functions implement a finite impulse response (FIR) filter for real-
valued samples with interpolation. The functions generate the interpolated
filtered response of the input data x and store the result in the output
vector z. The number of input samples is specified by the argument N and
the length of the output vector is D times more, where D is an interpolation
factor.

Impulse response h of M*D coefficients is stored in the polyphase form, i.e.
as a sequence of smaller filters h(d) of M coefficients for each of D
interpolation phases:
h[d*M+m] = h(d,m), where m = 0..M-1, d = 0..D-1.

The functions maintain the filter state in the structured variable state,
which must be declared and initialized before calling the function in the
same way as for the real FIR filter (see fir functions).

Input:
x[N]    input samples
N       number of input samples
state   filter state
Output:
z[N*D]  interpolated samples
state   updated filter state
Initialization macro:
See fir_init()
Domain:
Whole range
Restrictions:
N>0, M>0, D>1
N should be a multiple of 8
M should be a multiple of 4
delay line (d) should be aligned on 8-byte boundary
---------------------------------------------------------------------------*/
float32_t * fir_interpf_2x(float32_t * restrict z, float32_t * restrict delay, float32_t * restrict p, const float32_t * restrict h, const float32_t * restrict x, int M, int N)
{
        xtfloatx2 *restrict pZ;
        xtfloatx2 *restrict pX;
  const xtfloatx2 *restrict pHa;
  const xtfloatx2 *restrict pHb;
  const xtfloatx2 *restrict pDld;
        xtfloatx2 *restrict pDst;
  xtfloatx2 acc01a, acc23a, acc45a, acc67a;
  xtfloatx2 acc01b, acc23b, acc45b, acc67b;
  xtfloatx2 x_1, x01, x23, x45, x67;
  xtfloatx2 h0a, h0b;
  xtfloatx2 temp;
  xtfloat   s0, s1;
  ae_valign z_align;
  int n, m;

  NASSERT(x);
  NASSERT(z);
  NASSERT(N>0);
  NASSERT(M>0);

  /* set bounds of the delay line */
  WUR_AE_CBEGIN0((uintptr_t)(delay));
  WUR_AE_CEND0((uintptr_t)(delay + M));
  /* initialize pointers */
  pDst = (xtfloatx2 *)p;
  pX   = (xtfloatx2 *)x;
  pZ   = (xtfloatx2 *)z;
  z_align = AE_ZALIGN64();
  /* Process by 8 input samples (16 output samples) */
  __Pragma("loop_count min=1")
  for (n = 0; n < (N>>3); n++)
  {
      pHa = (const xtfloatx2 *)(h);
      pHb = (const xtfloatx2 *)(h+M);
      acc01a = acc23a = acc45a = acc67a = (xtfloatx2)(0.0f);
      acc01b = acc23b = acc45b = acc67b = (xtfloatx2)(0.0f);
      /* preload input samples */
      x01 = XT_LSX2I(pX, 0);
      x23 = XT_LSX2I(pX, 2*sizeof(float32_t));
      x45 = XT_LSX2I(pX, 4*sizeof(float32_t));
      x67 = XT_LSX2I(pX, 6*sizeof(float32_t));
      /* prepare for reverse loading of the delay line */
      pDld = pDst;
      AE_ADDCIRC32X2_XC(castxcc(ae_int32x2,pDld), -(int)sizeof(float32_t));

      /* kernel loop: compute by 1 tap for each sample */
      __Pragma("loop_count min=1")
      for (m = 0; m < M; m++)
      {
          /* load filter coefficients */
          XT_LSIP(s0, castxcc(xtfloat,pHa), sizeof(float32_t));
          XT_LSIP(s1, castxcc(xtfloat,pHb), sizeof(float32_t));
          h0a = s0;
          h0b = s1;
          /* multiply */
          XT_MADD_SX2(acc01a, h0a, x01);
          XT_MADD_SX2(acc23a, h0a, x23);
          XT_MADD_SX2(acc45a, h0a, x45);
          XT_MADD_SX2(acc67a, h0a, x67);
          XT_MADD_SX2(acc01b, h0b, x01);
          XT_MADD_SX2(acc23b, h0b, x23);
          XT_MADD_SX2(acc45b, h0b, x45);
          XT_MADD_SX2(acc67b, h0b, x67);
          /* load sample from the delay line *
           * and shift whole line            */
          XT_LSXC(s0, castxcc(xtfloat,pDld), -(int)sizeof(float32_t));
          x_1 = s0;
          x67 = XT_SEL32_LH_SX2(x45, x67);
          x45 = XT_SEL32_LH_SX2(x23, x45);
          x23 = XT_SEL32_LH_SX2(x01, x23);
          x01 = XT_SEL32_LH_SX2(x_1, x01);
      }
      /* save computed samples */
      temp = XT_SEL32_HH_SX2(acc01a, acc01b);    XT_SASX2IP(temp, z_align, pZ);
      temp = XT_SEL32_LL_SX2(acc01a, acc01b);    XT_SASX2IP(temp, z_align, pZ);
      temp = XT_SEL32_HH_SX2(acc23a, acc23b);    XT_SASX2IP(temp, z_align, pZ);
      temp = XT_SEL32_LL_SX2(acc23a, acc23b);    XT_SASX2IP(temp, z_align, pZ);
      temp = XT_SEL32_HH_SX2(acc45a, acc45b);    XT_SASX2IP(temp, z_align, pZ);
      temp = XT_SEL32_LL_SX2(acc45a, acc45b);    XT_SASX2IP(temp, z_align, pZ);
      temp = XT_SEL32_HH_SX2(acc67a, acc67b);    XT_SASX2IP(temp, z_align, pZ);
      temp = XT_SEL32_LL_SX2(acc67a, acc67b);    XT_SASX2IP(temp, z_align, pZ);
      /* update the delay line */
      XT_LSX2IP(x01, pX  , 2*sizeof(float32_t));
      XT_LSX2IP(x23, pX  , 2*sizeof(float32_t));
      XT_LSX2IP(x45, pX  , 2*sizeof(float32_t));
      XT_LSX2IP(x67, pX  , 2*sizeof(float32_t));
      XT_SSX2XC(x01, pDst, 2*sizeof(float32_t));
      XT_SSX2XC(x23, pDst, 2*sizeof(float32_t));
      XT_SSX2XC(x45, pDst, 2*sizeof(float32_t));
      XT_SSX2XC(x67, pDst, 2*sizeof(float32_t));
  }
  XT_SASX2POSFP(z_align, pZ);
  return (float32_t*)pDst;
} /* fir_interpf() */
#endif
