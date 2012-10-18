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

#ifndef _mach_io_h
#define _mach_io_h
/*
 * Machine independant io
 * For reading and writing to big-endian and little-endian files
 *
 * Routines available:
 *     be_write_int_1()
 *     be_write_int_2()
 *     be_write_int_4()
 *     be_read_int_1()
 *     be_read_int_2()
 *     be_read_int_4()
 *     le_write_int_1()
 *     le_write_int_2()
 *     le_write_int_4()
 *     le_read_int_1()
 *     le_read_int_2()
 *     le_read_int_4()
 *
 * All routine return:
 *    0 - an error has occurred during io operation
 *    1 - value successfully read or written
 */

#include <stdio.h>
#include "os.h"

/**********************************************************************/
/* IO for big-endian files                                            */
/**********************************************************************/

/*
 * Write a big-endian int_1
 */
extern int be_write_int_1(FILE *fp, uint_1 *i1);

/*
 * Write a big-endian int_2
 */
extern int be_write_int_2(FILE *fp, uint_2 *i2);

/*
 * Write a big-endian int_4
 */
extern int be_write_int_4(FILE *fp, uint_4 *i4);

/*
 * Read a big-endian int_1
 */
extern int be_read_int_1(FILE *fp, uint_1 *i1);

/*
 * Read a big-endian int_2
 */
extern int be_read_int_2(FILE *fp, uint_2 *i2);

/*
 * Read a big-endian int_4
 */
extern int be_read_int_4(FILE *fp, uint_4 *i4);

/**********************************************************************/
/* IO for little-endian files                                         */
/**********************************************************************/

/*
 * Write a little-endian int_1
 */
extern int le_write_int_1(FILE *fp, uint_1 *i1);

/*
 * Write a little-endian int_2
 */
extern int le_write_int_2(FILE *fp, uint_2 *i2);

/*
 * Write a little-endian int_4
 */
extern int le_write_int_4(FILE *fp, uint_4 *i4);

/*
 * Read a little-endian int_1
 */
extern int le_read_int_1(FILE *fp, uint_1 *i1);

/*
 * Read a little-endian int_2
 */
extern int le_read_int_2(FILE *fp, uint_2 *i2);

/*
 * Read a little-endian int_4
 */
extern int le_read_int_4(FILE *fp, uint_4 *i4);


#endif /* _mach_io_h */
