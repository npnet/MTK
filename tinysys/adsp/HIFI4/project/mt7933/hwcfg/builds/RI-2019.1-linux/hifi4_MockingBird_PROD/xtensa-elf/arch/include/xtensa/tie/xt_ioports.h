// Customer ID=14794; Build=0x87a3c; Copyright (c) 2017 Cadence Design Systems, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/* Definitions for the xt_ioports TIE package */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_ioports_HEADER
#define _XTENSA_xt_ioports_HEADER

/* parasoft-begin-suppress ALL "This file not MISRA checked." */

#ifdef __XTENSA__
#ifdef __XCC__

#ifndef _ASMLANGUAGE
#ifndef _NOCLANGUAGE
#ifndef __ASSEMBLER__

#include <xtensa/tie/xt_core.h>

/*
 * The following prototypes describe intrinsic functions
 * corresponding to TIE instructions.  Some TIE instructions
 * may produce multiple results (designated as "out" operands
 * in the iclass section) or may have operands used as both
 * inputs and outputs (designated as "inout").  However, the C
 * and C++ languages do not provide syntax that can express
 * the in/out/inout constraints of TIE intrinsics.
 * Nevertheless, the compiler understands these constraints
 * and will check that the intrinsic functions are used
 * correctly.  To improve the readability of these prototypes,
 * the "out" and "inout" parameters are marked accordingly
 * with comments.
 */

extern void _TIE_xt_ioports_CLRB_EXPSTATE(immediate bitindex);
extern unsigned _TIE_xt_ioports_READ_IMPWIRE(void);
extern unsigned _TIE_xt_ioports_RUR_EXPSTATE(void);
extern void _TIE_xt_ioports_SETB_EXPSTATE(immediate bitindex);
extern void _TIE_xt_ioports_WRMSK_EXPSTATE(unsigned art, unsigned ars);
extern void _TIE_xt_ioports_WUR_EXPSTATE(unsigned v);

#endif /*__ASSEMBLER__*/
#endif /*_NOCLANGUAGE*/
#endif /*_ASMLANGUAGE*/

#define CLRB_EXPSTATE _TIE_xt_ioports_CLRB_EXPSTATE
#define READ_IMPWIRE _TIE_xt_ioports_READ_IMPWIRE
#define RUR_EXPSTATE _TIE_xt_ioports_RUR_EXPSTATE
#define REXPSTATE _TIE_xt_ioports_RUR_EXPSTATE
#define RUR230 _TIE_xt_ioports_RUR_EXPSTATE
#define SETB_EXPSTATE _TIE_xt_ioports_SETB_EXPSTATE
#define WRMSK_EXPSTATE _TIE_xt_ioports_WRMSK_EXPSTATE
#define WUR_EXPSTATE _TIE_xt_ioports_WUR_EXPSTATE
#define WEXPSTATE _TIE_xt_ioports_WUR_EXPSTATE
#define WUR230 _TIE_xt_ioports_WUR_EXPSTATE

#ifndef RUR
#define RUR(NUM) RUR##NUM()
#endif

#ifndef WUR
#define WUR(VAL, NUM) WUR##NUM(VAL)
#endif

#endif /* __XCC__ */

#endif /* __XTENSA__ */


/* parasoft-end-suppress ALL "This file not MISRA checked." */

#endif /* !_XTENSA_xt_ioports_HEADER */
