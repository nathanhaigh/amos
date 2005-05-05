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
 * File: 	Read.c
 * Purpose:	Performs read/write IO on the Read data stucture.
 * Last update: 01/09/94
 */


/*
    The Read data type is designed so that it can hold a varying degree
    of information about sequences, yet have a single set of calls
    to access the data.

    There are plenty of assumptions around that both the number of
    bases and the number of points will fit into an int_2, a short.

*/

/* ---- Includes ---- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> /* Only need on windows for _O_BINARY */

#include "Read.h"

#ifdef IOLIB_ABI
# include "abi.h"
#endif
#ifdef IOLIB_SCF
# include "scf.h"
#endif
#ifdef IOLIB_ALF
# include "alf.h"
#endif
#ifdef IOLIB_PLN
# include "plain.h"
#endif
#ifdef IOLIB_ZTR
# include "ztr.h"
#endif
#ifdef IOLIB_CTF
# include "seqIOCTF.h"
#endif
#ifdef IOLIB_EXP
# include "expFileIO.h"
#endif
#ifdef USE_BIOLIMS
# include "spBiolims.h"
#endif

#include "xalloc.h"
#include "translate.h"
#include "traceType.h"
#include "misc.h"
#include "open_trace_file.h"

/*
 * Read a sequence from a file "fnin" of format "format". If "format" is 0
 * (ANY_FORMAT), we automatically determine the correct format.
 * Returns:
 *   Read *   for success
 *   NULLRead for failure
 */
Read *read_reading(char *fn, int format) {
    Read *read;
    FILE *fp;

#ifdef USE_BIOLIMS
    if( !strncmp(fn,BIOLIMS_TAG,strlen(BIOLIMS_TAG))){
	return spReadBiolimsReading(fn);
   }
#endif

    if (NULL == (fp = open_trace_file(fn, NULL))) {
	errout("'%s': couldn't open\n", fn);
	return NULL;
    }

    read = fread_reading(fp, fn, format);
    fclose(fp);

    return read;
}

/*
 * Write a sequence to a file "fn" of format "format". If "format" is 0,
 * we choose our favourite - SCF.
 *
 * Returns:
 *   0 for success
 *  -1 for failure
 */
int write_reading(char *fn, Read *read, int format) {
    Scf *scf;
    int ret;

    switch (format) {
    default:
	/* Defaults to SCF type */

#ifdef IOLIB_SCF
    case TT_SCF:
	scf = read2scf(read);
	ret = write_scf(scf, fn);
	scf_deallocate(scf);
	break;
#endif

#ifdef IOLIB_CTF
    case TT_CTF: {
	FILE *fp = fopen(fn, "wb");
	if (!fp)
	    return -1;
	ret = fwrite_ctf(fp, read); 
	fclose(fp);
	break;
    }
#endif

#ifdef IOLIB_ZTR
    case TT_ZTR:
    case TT_ZTR2: {
        ztr_t *ztr;
	FILE *fp = fopen(fn, "wb");
	if (!fp)
	    return -1;
	ztr = read2ztr(read);
	compress_ztr(ztr, 2);
	ret = fwrite_ztr(fp, ztr); 
	delete_ztr(ztr);
	fclose(fp);
	break;
    }
    case TT_ZTR1: {
        ztr_t *ztr;
	FILE *fp = fopen(fn, "wb");
	if (!fp)
	    return -1;
	ztr = read2ztr(read);
	compress_ztr(ztr, 1);
	ret = fwrite_ztr(fp, ztr); 
	delete_ztr(ztr);
	fclose(fp);
	break;
    }
    case TT_ZTR3: {
        ztr_t *ztr;
	FILE *fp = fopen(fn, "wb");
	if (!fp)
	    return -1;
	ztr = read2ztr(read);
	compress_ztr(ztr, 3);
	ret = fwrite_ztr(fp, ztr); 
	delete_ztr(ztr);
	fclose(fp);
	break;
    }
#endif

#ifdef IOLIB_ABI
    case TT_ABI:
	ret = write_abi(fn, read);
	break;
#endif

#ifdef IOLIB_ALF
    case TT_ALF:
	ret = write_alf(fn, read);
	break;
#endif

#ifdef IOLIB_EXP
    case TT_EXP: {
	Exp_info *e = read2exp(read, fn);
	
	if (NULL == e) {
	    fprintf(stderr, "Failed to create experiment file.\n");
	    return -1;
	} else {
	    FILE *fp;

	    if (NULL == (fp = fopen(fn, "w"))) {
		fprintf(stderr, "Couldn't open file '%s'\n", fn);
		return -1;
	    }

	    exp_print_file(fp, e);
	    fclose(fp);
	    exp_destroy_info(e);

	    ret = 0;
	}

	break;
    }
#endif
	
#ifdef IOLIB_PLN
    case TT_PLN:
	ret = write_pln(fn, read);
	break;
#endif
    }

    if (ret == 0)
	compress_file(fn);

    return ret;
}

/*
 * Read a sequence from a FILE *fp of format "format". If "format" is 0
 * (ANY_FORMAT), we automatically determine the correct format.
 * We still pass a filename 'fn' although this isn't used other than for
 * filling in the read->trace_name field.
 *
 * NB this function should NOT be used when Biolims support is required
 * (as biolims readings are not stored in a file)
 *
 * Returns:
 *   Read *   for success
 *   NULLRead for failure
 */
Read *fread_reading(FILE *fp, char *fn, int format) {
    Read *read;
    FILE *newfp;

    if (!fn)
	fn = "(unknown)";

    newfp = freopen_compressed(fp, NULL);
    if (newfp != fp) {
	fp = newfp;
    } else {
	newfp = NULL;
    }

#ifdef _WIN32
    /*
     * jkb 16/05/00 comment below
     *
     * On windows "prog < file.abi" will work wrongly (compared to
     * "prog file.abi") because windows is rather stupid. It treats ascii
     * and binary streams differently, it considers stdin to be ascii unless
     * told otherwise, and it can only be told otherwise by using non-ansi
     * windows-specific function calls.
     */
    if (format != TT_EXP && format != TT_PLN)
	_setmode(_fileno(fp), _O_BINARY);
#endif

    if (format == TT_ANY) {
	format = fdetermine_trace_type(fp);
	rewind(fp);
    }

    switch (format) {
    case TT_UNK:
    case TT_ERR:
	errout("File '%s' has unknown trace type\n", fn);
	read = NULLRead;
	break;

#ifdef IOLIB_SCF
    case TT_SCF: {
        Scf *scf;
	scf = fread_scf(fp);

	if (scf) {
	    read = scf2read(scf);
	    scf_deallocate(scf);
	} else
	    read = NULLRead;

	break;
    }
#endif

#ifdef IOLIB_CTF
    case TT_CTF:
	read = fread_ctf(fp);
	break;
#endif

#ifdef IOLIB_ZTR
    case TT_ZTR:
    case TT_ZTR1:
    case TT_ZTR2:
    case TT_ZTR3: {
        ztr_t *ztr;

	if ((ztr = fread_ztr(fp))) {
	    uncompress_ztr(ztr);
	    read = ztr2read(ztr);
	    delete_ztr(ztr);
	} else {
	    read = NULLRead;
	}
	break;
    }
#endif

#ifdef IOLIB_ABI
    case TT_ABI:
	read = fread_abi(fp);
	break;
#endif

#ifdef IOLIB_ALF
    case TT_ALF:
	read = fread_alf(fp);
	break;
#endif

#ifdef IOLIB_EXP
    case TT_EXP: {
	/* FIXME: we shouldn't redirect like this */
	Exp_info *e = exp_fread_info(fp);
	
	read = e ? exp2read(e,fn) : NULLRead;
	break;
    }
#endif

#ifdef IOLIB_PLN
    case TT_PLN:
	read = fread_pln(fp);
	break;
#endif

    default:
	errout("Unknown format %d specified to read_reading()\n", format);
	read = NULLRead;
    }

    if (read != NULLRead && (read->trace_name = (char *)xmalloc(strlen(fn)+1)))
	strcpy(read->trace_name, fn);

    if (newfp) fclose(newfp);

    return read;
}

/*
 * Write a sequence to a FILE *fp of format "format". If "format" is 0,
 * we choose our favourite - SCF.
 *
 * Returns:
 *   0 for success
 *  -1 for failure
 */
int fwrite_reading(FILE *fp, Read *read, int format) {
    int r = -1;

#ifdef _WIN32
    /*
     * jkb 09/06/00 comment below
     *
     * On windows "prog > file.scf" will work wrongly (compared to
     * "prog file.scf") because windows is rather stupid. It treats ascii
     * and binary streams differently, it considers stdout to be ascii unless
     * told otherwise, and it can only be told otherwise by using non-ansi
     * windows-specific function calls.
     */
    if (format != TT_EXP && format != TT_PLN)
	_setmode(_fileno(fp), _O_BINARY);
#endif

    switch (format) {
    default:
	/* Defaults to SCF type */

#ifdef IOLIB_SCF
    case TT_SCF: {
        Scf *scf;
	scf = read2scf(read);
	r = fwrite_scf(scf, fp);
	scf_deallocate(scf);
	break;
    }
#endif

#ifdef IOLIB_CTF
    case TT_CTF:
	r = fwrite_ctf(fp, read); 
	break;
#endif

#ifdef IOLIB_ZTR
    case TT_ZTR:
    case TT_ZTR2: {
        ztr_t *ztr;
	ztr = read2ztr(read);
	compress_ztr(ztr, 2);
	r = fwrite_ztr(fp, ztr); 
	delete_ztr(ztr);
	break;
    }
    case TT_ZTR1: {
        ztr_t *ztr;
	ztr = read2ztr(read);
	compress_ztr(ztr, 1);
	r = fwrite_ztr(fp, ztr); 
	delete_ztr(ztr);
	break;
    }
    case TT_ZTR3: {
        ztr_t *ztr;
	ztr = read2ztr(read);
	compress_ztr(ztr, 3);
	r = fwrite_ztr(fp, ztr); 
	delete_ztr(ztr);
	break;
    }
#endif

#ifdef IOLIB_ABI
    case TT_ABI:
	/*return fwrite_abi(fp, read); */
	break;
#endif

#ifdef IOLIB_ALF
    case TT_ALF:
	/* return fwrite_alf(fp, read); */
	break;
#endif

#ifdef IOLIB_EXP
    case TT_EXP: {
	Exp_info *e = read2exp(read, read->ident ? read->ident : "unknown");
	
	if (NULL == e) {
	    fprintf(stderr, "Failed to create experiment file.\n");
	    r = -1;
	} else {
	    exp_print_file(fp, e);
	    exp_destroy_info(e);
	    r = 0;
	}
	break;
    }
#endif

#ifdef IOLIB_PLN
    case TT_PLN:
	r = fwrite_pln(fp, read);
	break;
#endif
    }

    if (r == 0) {
	fcompress_file(fp);
    }

    return r;
}
