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

#include <errno.h>

/* DSP Library API */
#include "NatureDSP_Signal.h"
/* Common helper macros. */
#include "common.h"
/* Inline functions for floating-point exceptions and environment control. */
#include "__fenv.h"
/* Tables */
#include "log2f_tbl.h"
#include "sqrt2f_tbl.h"
/* +/-Infinity, single precision */
#include "inff_tbl.h"
/* sNaN/qNaN, single precision. */
#include "nanf_tbl.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(float32_t,scl_log2f,(float32_t x))
#else
/* If non-zero, set errno and raise floating-point exceptions on errors. */
#define SCL_LOG2F_ERRH     1

/*===========================================================================
  Scalar matematics:
  scl_log              Logarithm 
===========================================================================*/

/*-------------------------------------------------------------------------
  Logarithm:
  Different kinds of logarithm (base 2, natural, base 10). Fixed point 
  functions represent results in Q25 format or return 0x80000000 on negative 
  of zero input.

  Precision:
  32x32  32-bit inputs, 32-bit outputs
  f      floating point

  Accuracy :
  vec_log2_32x32,scl_log2_32x32       730 (2.2e-5)
  vec_logn_32x32,scl_logn_32x32       510 (1.5e-5)
  vec_log10_32x32,scl_log10_32x32     230 (6.9e-6)
  floating point                      2 ULP

  NOTES:
  1.  Scalar Floating point functions are compatible with standard ANSI C routines 
      and set errno and exception flags accordingly.
  2.  Floating point functions limit the range of allowable input values:
      A) If x<0, the result is set to NaN. In addition, scalar floating point
         functions assign the value EDOM to errno and raise the "invalid" 
         floating-point exception.
      B) If x==0, the result is set to minus infinity. Scalar floating  point
         functions assign the value ERANGE to errno and raise the "divide-by-zero"
         floating-point exception.

  Input:
  x[N]  input data, Q16.15 or floating point 
  N     length of vectors
  Output:
  y[N]  result, Q25 or floating point 

  Restriction:
  x,y should not overlap

  Scalar versions:
  ----------------
  return result in Q25 or floating point
-------------------------------------------------------------------------*/

float32_t scl_log2f (float32_t x)
{
  /*
   * Reference C code for SCL_LOG2F_ERRH == 0:
   *
   *   float32_t y;
   *   int e;
   *  
   *   if ( x<0           ) return ( qNaNf.f     );
   *   if ( x==0          ) return ( minusInff.f );
   *   if ( x==plusInff.f ) return ( x           );
   *  
   *   x=frexpf(x,&e);
   *   if(x<sqrt0_5f.f) { x=x*2; e--; }
   *  
   *   x=1.0f-x;
   *   y=log2f_tbl[0].f;
   *   y=log2f_tbl[1].f+x*y;
   *   y=log2f_tbl[2].f+x*y;
   *   y=log2f_tbl[3].f+x*y;
   *   y=log2f_tbl[4].f+x*y;
   *   y=log2f_tbl[5].f+x*y;
   *   y=log2f_tbl[6].f+x*y;
   *   y=log2f_tbl[7].f+x*y;
   *   y=log2f_tbl[8].f+x*y;
   *   y=log2f_tbl[9].f+x*y;
   *   y=x*y;
   *   y=y+(float32_t)e;
   *   return y;
   */

  /* Input value; output value; fractional part and its 2nd and 4th powers; auxiliary var. */
  xtfloatx2 x0, x1, y0, fr0, fr1, fr2, t;
  /* Fixed-point representation of fractional part; exponential part; exponent bias */
  ae_int32x2 fr0_i, ex0, ex1, bias;
  /* Polynomial value; polynomial coefficients */
  xtfloatx2 g, cf0, cf1, cf2, cf3, cf4, cf5, cf6, cf7, cf8, cf9;
  /* Floating-point zero constant. */
  xtfloatx2 zero_f = (xtfloatx2)(0.0f);
  /* Is a subnormal; is less than 2^0.5; is positive infinity */
  xtbool2 b_subn, b_ltsqr, b_inf;

#if SCL_LOG2F_ERRH == 0
  /* Is a NaN or is less than zero; is equal to zero. */
  xtbool2 b_ultz, b_eqz;
#else
  /* Floating-point Status and Control Register values. */
  ae_int64 fstate;
#endif

  x0 = (xtfloatx2)x;

#if SCL_LOG2F_ERRH != 0
  if ( xtbool2_extract_0( XT_UN_SX2( x0, x0 ) ) )
  {
    __Pragma( "frequency_hint never" );
    errno = EDOM;
    return (qNaNf.f);
  }
  else if ( xtbool2_extract_0( XT_OLT_SX2( x0, zero_f ) ) )
  {
    __Pragma( "frequency_hint never" );
    errno = EDOM;
    __feraiseexcept( FE_INVALID );
    return (qNaNf.f);
  }
  else if ( xtbool2_extract_0( XT_OEQ_SX2( x0, zero_f ) ) )
  {
    __Pragma( "frequency_hint never" );
    errno = ERANGE;
    __feraiseexcept( FE_DIVBYZERO );
    return (minusInff.f);
  }
#endif

  /*
   * frexpf(x)
   */

  /* Compare with smallest positive normal number 2^-126 */
  b_subn = XT_OLT_SX2( x0, XT_AE_MOVXTFLOATX2_FROMINT32X2(0x00800000) );

  t = (xtfloatx2)(1.0f);
  /* Multiply subnormals by 2^23 */
  XT_MOVT_SX2( t, XT_AE_MOVXTFLOATX2_FROMINT32X2(0x4b000000), b_subn );
#if SCL_LOG2F_ERRH != 0
  /* Sample floating-point exception flags. */
  fstate = XT_AE_MOVVFCRFSR();
#endif
  x1 = XT_MUL_SX2( x0, t );
  fr0_i = XT_AE_MOVINT32X2_FROMXTFLOATX2( x1 );

  bias = 127-1;
  AE_MOVT32X2( bias, 127-1+23, b_subn );
  ex0 = AE_SRLI32( fr0_i, 23 );
  ex0 = AE_SUB32( ex0, bias );

  fr0_i = AE_AND32( fr0_i, (1<<23)-1 );
  fr0_i = AE_OR32( fr0_i, 126<<23 );

  fr0 = XT_AE_MOVXTFLOATX2_FROMINT32X2( fr0_i );

  fr1 = XT_MUL_SX2( fr0, (xtfloatx2)2.0f );
  ex1 = AE_SUB32( ex0, AE_MOVI(1) );

  b_ltsqr = XT_OLT_SX2( fr0, sqrt0_5f.f );
  XT_MOVT_SX2( fr0, fr1, b_ltsqr );
  AE_MOVT32X2( ex0, ex1, b_ltsqr );
  y0 = XT_FLOAT_SX2( ex0, 0 );

  fr0 = XT_SUB_SX2( (xtfloatx2)1.0f, fr0 );
  fr2 = XT_MUL_SX2( fr0, fr0 );

#if SCL_LOG2F_ERRH != 0
  /* Suppress spurious exception flags and restore original status flags. */
  XT_AE_MOVFCRFSRV( fstate );
#endif

  /*
   * Use a combination of Estrin's method and Horner's scheme to evaluate
   * the polynomial.
   */

  cf0 = (xtfloatx2)log2f_tbl[0].f; cf1 = (xtfloatx2)log2f_tbl[1].f;
  cf2 = (xtfloatx2)log2f_tbl[2].f; cf3 = (xtfloatx2)log2f_tbl[3].f;
  cf4 = (xtfloatx2)log2f_tbl[4].f; cf5 = (xtfloatx2)log2f_tbl[5].f;
  cf6 = (xtfloatx2)log2f_tbl[6].f; cf7 = (xtfloatx2)log2f_tbl[7].f;
  cf8 = (xtfloatx2)log2f_tbl[8].f; cf9 = (xtfloatx2)log2f_tbl[9].f;

  XT_MADDN_SX2( cf1, cf0, fr0 );
  XT_MADDN_SX2( cf3, cf2, fr0 );
  XT_MADDN_SX2( cf5, cf4, fr0 );
  XT_MADDN_SX2( cf7, cf6, fr0 );
  XT_MADDN_SX2( cf9, cf8, fr0 );

  g = cf1;
  XT_MADDN_SX2( cf3, g, fr2 ); g = cf3;
  XT_MADDN_SX2( cf5, g, fr2 ); g = cf5;
  XT_MADDN_SX2( cf7, g, fr2 ); g = cf7;
  XT_MADDN_SX2( cf9, g, fr2 ); g = cf9;

  /* 
   * Compute y = fr*g + ex
   */

  XT_MADDN_SX2( y0, g, fr0 );

  /*
   * Process the special cases.
   */

#if SCL_LOG2F_ERRH != 0
  b_inf = XT_OEQ_SX2( x0, plusInff.f );
  XT_MOVT_SX2( y0, plusInff.f, b_inf ); /* x==Inf -> y=Inf */
#else
  b_inf = XT_OEQ_SX2( x0, plusInff.f );
  b_ultz = XT_ULT_SX2( x0, zero_f );
  b_eqz = XT_OEQ_SX2( x0, zero_f );

  XT_MOVT_SX2( y0, plusInff.f , b_inf  ); /* x==Inf -> y=Inf */
  XT_MOVT_SX2( y0, minusInff.f, b_eqz  ); /* x==0.f -> y=-Inf */
  XT_MOVT_SX2( y0, qNaNf.f    , b_ultz ); /* x<0 || isnan(x) -> y=qNaN */
#endif

  return ( XT_HIGH_S( y0 ) );

} /* scl_log2f() */
#endif
