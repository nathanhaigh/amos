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

/* 
 * Title:       seqIOCTF
 * 
 * File: 	seqIOCTF.c
 * Purpose:	Reading/writing of CTF sequences
 * Last update: March 2000
 *
 * Change log:
 * Created mieg, march 2000, importing code from wabi/ctftrace.c
 */


/* ---- Imports ---- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "Read.h"
#include "seqIOCTF.h"
#include "abi.h"
#include "mach-io.h"
#include "xalloc.h"

/* ---- Constants ---- */

/*
 * Read the CTF format sequence from FILE *fp into a Read structure.

 * Returns:
 *   Read *	- Success, the Read structure read.
 *   NULLRead	- Failure.
 */
Read *fread_ctf (FILE *fp) {
    Read *read = ctfFRead (fp) ;

    return read ;
}

/*
 * Read the CTF format sequence from file 'fn' into a Read structure.
 */

Read *read_ctf (char *fn) {
    Read *read;
    FILE *fp;

    /* Open file */
    if ((fp = fopen(fn, "rb")) == NULL)
	return NULLRead;

    read = fread_ctf(fp);
    fclose(fp);

    if (read && (read->trace_name = (char *)xmalloc(strlen(fn)+1)))
	strcpy(read->trace_name, fn);

    return read;
}
    

/*
 * Write to an CTF file - unsupported.
 */
/* ARGSUSED */
int fwrite_ctf (FILE *fp, Read *read) {
  return ctfFWrite (fp, read) ;
}

/*
 * Write to an CTF file 
 */
/* ARGSUSED */
int write_ctf(char *fn, Read *read) {
  FILE *fp;
  
  /* Open file */
  if ((fp = fopen(fn, "wb")) == NULL)
    return -1 ;
  
  fwrite_ctf (fp, read) ;
  fclose(fp);

  return 0 ;
}

