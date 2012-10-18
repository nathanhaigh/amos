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

#ifndef _seqIOPlain_h
#define _seqIOPlain_h


/* 
 * Title:       seqIOPlain
 *
 * File: 	 seqIOPlain.h
 * Purpose:	 IO of plain sequences
 * Last update:  Fri 9th September 1994
 */




/* ---- Imports ---- */


#include "Read.h"


/* ---- Exports ---- */


/*
 * Read the plain format sequence with name `fn' into a Read structure.
 * All printing characters (as defined by ANSII C `isprint')
 * are accepted, but `N's are translated to `-'s.
 *
 * Returns:
 *   Read *     - Success, the Read structure read.
 *   NULLRead   - Failure.
 */
extern Read *read_pln(char *fn);
extern Read *fread_pln(FILE *fp);

#endif  /*_seqIOPlain_h*/
