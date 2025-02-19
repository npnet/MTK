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
    Complex block FIR filter, 32x16-bit
    C code optimized for HiFi4
  IntegrIT, 2006-2014
*/


/* Portable data types. */
#include "NatureDSP_types.h"
/* Signal Processing Library API. */
#include "NatureDSP_Signal.h"
/* Common utility and macros declarations. */
#include "common.h"
#include "cxfir32x16_common.h"

/*-------------------------------------------------------------------------
  Complex Block FIR Filter
  Computes a complex FIR filter (direct-form) using complex IR stored in 
  vector h. The complex data input is stored in vector x. The filter output 
  result is stored in vector r. The filter calculates N output samples using
  M coefficients, requires last N-1 samples in the delay line which is 
  updated in circular manner for each new sample. Real and imaginary parts 
  are interleaved and real parts go first (at even indexes).
  User has an option to set IR externally or copy from original location 
  (i.e. from the slower constant memory). In the first case, user is 
  responsible for right alignment, ordering and zero padding of filter 
  coefficients - usually array is composed from zeroes (left padding), 
  reverted IR and right zero padding.

  Precision: 
  32x16    32-bit data, 16-bit coefficients, 32-bit outputs
  32x32    32-bit data, 32-bit coefficients, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f        floating point

  Input:
  h[M]   complex filter coefficients; h[0] is to be multiplied with the newest 
         sample , Q31, Q15, floating point
  x[N]   input samples, Q31, floating point
  N      length of sample block (in complex samples) 
  M      length of filter 
  extIR  if zero, IR is copied from original location, otherwise 
         not but user should keep alignment, order of coefficients 
         and zero padding requirements shown below
  Output:
  y[N]   output samples, Q31, floating point

  Alignment, ordering and zero padding for external IR  (extIR!=0)
  -----------------+----------+--------------+--------------+----------------
  Function	       |Alignment,|Left zero     |   Coefficient| Right zero 
                   | bytes    |padding, bytes|   order      | padding, bytes
  -----------------+----------+--------------+--------------+----------------
  cxfir32x16_init  |     8    |    4         |  inverted    |  4
  cxfir32x32_init  |     8    |    0         | inverted and |  0
                   |          |              | conjugated   |
  cxfir32x32ep_init|     8    |    0         | inverted and |  0
                   |          |              | conjugated   |
  cxfirf_init      |     8    |    0         | direct       |  0
  -----------------+----------+--------------+--------------+----------------

  Restriction:
  x,y - should not overlap
  x,h - aligned on a 8-bytes boundary
  N,M - multiples of 4
-------------------------------------------------------------------------*/
void cxfir32x16_process( cxfir32x16_handle_t _cxfir, 
                         complex_fract32 * restrict  y,
                   const complex_fract32 * restrict  x, int N )
{
  cxfir32x16_ptr_t cxfir = (cxfir32x16_ptr_t)_cxfir;

  const ae_int16x4   *          C;
        ae_int32x2 * restrict D_wr;
  const ae_int32x2 *          D_rd;
  const ae_int32x2 *          X;
        ae_f32     * restrict Y;

  ae_int32x2 t0ri, t1ri, t2ri, t3ri, t4ri;
  ae_f32x2   d0ri, d1ri, d2ri, d3ri, d4ri;
  ae_f64     q0r, q0i, q1r, q1i, q2r, q2i, q3r, q3i;
  ae_int16x4 c01;

  int M;
  int m, n;

  ASSERT( cxfir && cxfir->magic == CXFIR32X16_MAGIC && y && x );

  ASSERT( IS_ALIGN( x ) );

  M = cxfir->M;

  ASSERT( !( M & 3 ) && !( N & 3 ) );

  ASSERT( IS_ALIGN( cxfir->delayLine                   ) &&
          IS_ALIGN( cxfir->delayLine + cxfir->delayLen ) &&
          IS_ALIGN( cxfir->coef ) );

  //
  // Setup pointers and circular delay line buffer.
  //

  X    = (const ae_int32x2 *)x;
  Y    = (      ae_f32     *)y;
  D_wr = (      ae_int32x2 *)(cxfir->delayLine+cxfir->wrIx);

  // Reset the coefficients pointer. Now it looks at the tap corresponding
  // to the oldest sample in the delay line.
  C = (const ae_int16x4*)cxfir->coef;

  WUR_AE_CBEGIN0( (uintptr_t)( cxfir->delayLine                     ) );
  WUR_AE_CEND0  ( (uintptr_t)( cxfir->delayLine + 2*cxfir->delayLen ) );
  WUR_AE_CBEGIN1( (uintptr_t)( cxfir->coef                   ) );
  WUR_AE_CEND1  ( (uintptr_t)( cxfir->coef + 2*cxfir->M +4 ) );
  //
  // Break the input signal into 4-samples blocks. For each block, store 4
  // samples to the delay line and compute the filter response.
  //

  for ( n=0; n<(N>>2); n++ )
  {
    // Load 4 complex input samples.
    // Q31
    AE_L32X2_IP( t0ri, X, +8 );
    AE_L32X2_IP( t1ri, X, +8 );
    AE_L32X2_IP( t2ri, X, +8 );
    AE_L32X2_IP( t3ri, X, +8 );

    // Store 4 complex samples to the delay line buffer with circular address
    // update.
    // Q31
    AE_S32X2_XC( t0ri, D_wr, +8 );
    AE_S32X2_XC( t1ri, D_wr, +8 );
    AE_S32X2_XC( t2ri, D_wr, +8 );
    AE_S32X2_XC( t3ri, D_wr, +8 );

    // Circular buffer pointer looks at the oldest sample: M+4 samples back from
    // the newest one.
    D_rd = D_wr;

    // Load 3 oldest samples from the delay line.
    // Q31
    AE_L32X2_XC( t0ri, D_rd, +8 );
    AE_L32X2_XC( t1ri, D_rd, +8 );
    AE_L32X2_XC( t2ri, D_rd, +8 );

    d0ri = ( t0ri );
    d1ri = ( t1ri );
    d2ri = ( t2ri );

    //
    // Inner loop prologue: process the first 2 taps for each of 4 complex
    // accumulators.
    //

    // Load next 2 samples. Altogether we have 5 complex samples residing in
    // 5 AE registers.
    // Q31
    AE_L32X2_XC( t3ri, D_rd, +8 );
    AE_L32X2_XC( t4ri, D_rd, +8 );

    d3ri = ( t3ri );
    d4ri = ( t4ri );



    // Load 2 complex tap coefficients. Coefficients vector format:
    // 3(MSW)  2    1   0(LSW)
    //  c0re  c0im c1re c1im    Q15
    AE_L16X4_XC1(c01, C, +8);

    // Initialize the accumulators by the higher coefficient product.
    // Q16.47 <- [ Q31*Q15 + 1 ] + [ Q31*Q15 + 1 ]
    q0r = AE_MULZASFD32X16_H3_L2( d0ri, c01 ); // d0re*c0re - d0im*c0im
    q0i = AE_MULZAAFD32X16_H2_L3( d0ri, c01 ); // d0re*c0im + d0im*c0re
    q1r = AE_MULZASFD32X16_H3_L2( d1ri, c01 ); // d1re*c0re - d1im*c0im
    q1i = AE_MULZAAFD32X16_H2_L3( d1ri, c01 ); // d1re*c0im + d1im*c0re
    q2r = AE_MULZASFD32X16_H3_L2( d2ri, c01 ); // d2re*c0re - d2im*c0im
    q2i = AE_MULZAAFD32X16_H2_L3( d2ri, c01 ); // d2re*c0im + d2im*c0re
    q3r = AE_MULZASFD32X16_H3_L2( d3ri, c01 ); // d3re*c0re - d3im*c0im
    q3i = AE_MULZAAFD32X16_H2_L3( d3ri, c01 ); // d3re*c0im + d3im*c0re

    // Accumulate the lower coefficient product.
    // Q16.47 <- Q16.47 + [ Q31*Q15 + 1 ] + [ Q31*Q15 + 1 ]
    AE_MULASFD32X16_H1_L0( q0r, d1ri, c01 );   // d1re*c1re - d1im*c1im
    AE_MULAAFD32X16_H0_L1( q0i, d1ri, c01 );   // d1re*c1im + d1im*c1re
    AE_MULASFD32X16_H1_L0( q1r, d2ri, c01 );   // d2re*c1re - d2im*c1im
    AE_MULAAFD32X16_H0_L1( q1i, d2ri, c01 );   // d2re*c1im + d2im*c1re
    AE_MULASFD32X16_H1_L0( q2r, d3ri, c01 );   // d3re*c1re - d3im*c1im
    AE_MULAAFD32X16_H0_L1( q2i, d3ri, c01 );   // d3re*c1im + d3im*c1re
    AE_MULASFD32X16_H1_L0( q3r, d4ri, c01 );   // d4re*c1re - d4im*c1im
    AE_MULAAFD32X16_H0_L1( q3i, d4ri, c01 );   // d4re*c1im + d4im*c1re

    // 2 taps are done. Move 2 complex input samples out of the registers file.
    d0ri = d2ri; d1ri = d3ri; d2ri = d4ri;

    __Pragma("loop_count min=2")
    for ( m=0; m<(M>>1); m++ )
    {
      // Load next 2 samples. Altogether we have 5 complex samples residing in
      // 5 AE registers.
      // Q31
      AE_L32X2_XC( t3ri, D_rd, +8 );
      AE_L32X2_XC( t4ri, D_rd, +8 );

      d3ri = ( t3ri );
      d4ri = ( t4ri );

      // Load 2 complex tap coefficients.
      // Q15
      AE_L16X4_XC1(c01, C, +8);

      // Accumulate the higher coefficient product.
      // Q16.47 <- Q16.47 + [ Q31*Q15 + 1 ] + [ Q31*Q15 + 1 ]
      AE_MULASFD32X16_H3_L2( q0r, d0ri, c01 ); // d0re*c0re - d0im*c0im
      AE_MULAAFD32X16_H2_L3( q0i, d0ri, c01 ); // d0re*c0im + d0im*c0re
      AE_MULASFD32X16_H3_L2( q1r, d1ri, c01 ); // d1re*c0re - d1im*c0im
      AE_MULAAFD32X16_H2_L3( q1i, d1ri, c01 ); // d1re*c0im + d1im*c0re
      AE_MULASFD32X16_H3_L2( q2r, d2ri, c01 ); // d2re*c0re - d2im*c0im
      AE_MULAAFD32X16_H2_L3( q2i, d2ri, c01 ); // d2re*c0im + d2im*c0re
      AE_MULASFD32X16_H3_L2( q3r, d3ri, c01 ); // d3re*c0re - d3im*c0im
      AE_MULAAFD32X16_H2_L3( q3i, d3ri, c01 ); // d3re*c0im + d3im*c0re

      // Accumulate the lower coefficient product.
      // Q16.47 <- Q16.47 + [ Q31*Q15 + 1 ] + [ Q31*Q15 + 1 ]
      AE_MULASFD32X16_H1_L0( q0r, d1ri, c01 ); // d1re*c1re - d1im*c1im
      AE_MULAAFD32X16_H0_L1( q0i, d1ri, c01 ); // d1re*c1im + d1im*c1re
      AE_MULASFD32X16_H1_L0( q1r, d2ri, c01 ); // d2re*c1re - d2im*c1im
      AE_MULAAFD32X16_H0_L1( q1i, d2ri, c01 ); // d2re*c1im + d2im*c1re
      AE_MULASFD32X16_H1_L0( q2r, d3ri, c01 ); // d3re*c1re - d3im*c1im
      AE_MULAAFD32X16_H0_L1( q2i, d3ri, c01 ); // d3re*c1im + d3im*c1re
      AE_MULASFD32X16_H1_L0( q3r, d4ri, c01 ); // d4re*c1re - d4im*c1im
      AE_MULAAFD32X16_H0_L1( q3i, d4ri, c01 ); // d4re*c1im + d4im*c1re

      // 2 taps are done. Move 2 complex input samples out of the registers file
      d0ri = d2ri; d1ri = d3ri; d2ri = d4ri;
    }

    // Convert and save 4 outputs to the output array.
    // 2xQ31 <- 2xQ16.47 - 16 w/ rounding and saturation.
    AE_S32RA64S_IP( q0r, Y, +4 );
    AE_S32RA64S_IP( q0i, Y, +4 );
    AE_S32RA64S_IP( q1r, Y, +4 );
    AE_S32RA64S_IP( q1i, Y, +4 );
    AE_S32RA64S_IP( q2r, Y, +4 );
    AE_S32RA64S_IP( q2i, Y, +4 );
    AE_S32RA64S_IP( q3r, Y, +4 );
    AE_S32RA64S_IP( q3i, Y, +4 );
  }
  cxfir->wrIx = (int)((int32_t *)D_wr - cxfir->delayLine) ;

} // cxfir32x16_process()
