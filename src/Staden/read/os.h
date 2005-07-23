/*
 * Copyright (c) Medical Research Council 1994. All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * this copyright and notice appears in all copies.
 *
 * This file was written by James Bonfield, Simon Dear, Rodger Staden,
 * as part of the Staden Package at the MRC Laboratory of Molecular
 * Biology, Hills Road, Cambridge, CB2 2QH, United Kingdom.
 *
 * MRC disclaims all warranties with regard to this software.
 */

#ifndef _OS_H_
#define _OS_H_

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <inttypes.h>
#include <limits.h>


/* Integer sizes */
typedef unsigned char uint1;
typedef signed char   int1;
typedef uint16_t      uint2;
typedef int16_t       int2;
typedef uint32_t      uint4;
typedef int32_t       int4;

/* For backwards compatibility */
typedef  int1   int_1;
typedef uint1  uint_1;
typedef  int2   int_2;
typedef uint2  uint_2;
typedef  int4   int_4;
typedef uint4  uint_4;

/* Fortran types */
typedef int4 f_int;
typedef int4 f_implicit;
typedef void f_proc_ret; /* FORTRAN procedure return value */

/* For backwards compatibility */
typedef int4 int_f;		/* f_int */
typedef int4 int_fl;		/* f_implicit */
#define f_proc_return() return /* (f_proc_ret) 0 */

#define MAXINT4 (INT_MAX)
#define MAXINT2 (SHRT_MAX)

#define False 0
#define True 1

/*
 * Our new swap runs at the same speed on Ultrix, but substantially faster
 * (300% for swap_int4, ~50% for swap_int2) on an Alpha (due to the lack of
 * decent 'char' support).
 *
 * They also have the ability to swap in situ (src == dst). Newer code now
 * relies on this so don't change back!
 */
#define swap_int4(src, dst) \
    dst = ((src & 0x000000ff) << 24) + \
          ((src & 0x0000ff00) <<  8) + \
          ((src & 0x00ff0000) >>  8) + \
          ((src & 0xff000000) >> 24)

#define swap_int2(src, dst) \
    dst = ((src & 0x00ff) << 8) + \
          ((src & 0xff00) >> 8)

/*
 * Slightly updated swap_int? routines that return results rather than
 * swapping from source to destination.
 */
#define iswap_int4(x) \
    (((x & 0x000000ff) << 24) + \
     ((x & 0x0000ff00) <<  8) + \
     ((x & 0x00ff0000) >>  8) + \
     ((x & 0xff000000) >> 24))

#define iswap_int2(x) \
    (((x & 0x00ff) << 8) + \
     ((x & 0xff00) >> 8))

/*
 * Macros to specify that data read in is of a particular endianness.
 * The macros here swap to the appropriate order for the particular machine
 * running the macro and return the new answer. These may also be used when
 * writing to a file to specify that we wish to write in (eg) big endian
 * format.
 *
 * This leads to efficient code as most of the time these macros are
 * trivial.
 */
#ifdef WORDS_BIGENDIAN
# define be_int4(x) (x)
# define be_int2(x) (x)
# define be_int1(x) (x)
# define le_int4(x) iswap_int4((x))
# define le_int2(x) iswap_int2((x))
# define le_int1(x) (x)
#else
# define SP_LITTLE_ENDIAN 1
# define be_int4(x) iswap_int4((x))
# define be_int2(x) iswap_int2((x))
# define be_int1(x) (x)
# define le_int4(x) (x)
# define le_int2(x) (x)
# define le_int1(x) (x)
#endif

#endif /* _OS_H_ */
