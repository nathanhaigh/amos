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
 * Title:       seqIOABI
 * 
 * File: 	seqIOABI.c
 * Purpose:	Reading (not writing) of ABI sequences
 * Last update: Fri Sep 02, 1994
 *
 * Change log:
 * 27/11/90 SD     writeSeqABI() outputs header to sequence file:
 * format: ;{noOfBases}{leftCutOff}{basesWritten}{type}{tracefile}
 * eg:     ;   867    45    383ABI a09b7.s1RES
 * 28.11.90 SD  put undesirables under STLOUIS compilation flag
 * 11.12.90 SD  new static function tail to find file name in path name
 * 02.01.91 SD  Merged with St.L version
 * 15.01.91 SD  New include added (opp.h)
 * 30.07.91 SD  Those ole FWO_ field blues
 * 17.09.91 LFW changed STLOUIS compilation flag to SAVE_EDITS
 *              and AUTO_CLIP
 * 25.10.91 SD  Machine independant I/O...removed BIGENDIAN flag
 * 21.07.92 LFW Added finding of primer position
 * 11.11.92 LFW added section to actually check that the trace it
 *              is trying to open is an ALF file using traceType sub
 * 10.11.92 SD  FWO_ and S/N% interpretation. Comments for information
 *              window.
 * 05-Jul-93 SD Added code to check base positions are in order and adjust
 *              them if they are not
 * 02.09.94 JKB Change to use Read instead of Seq library.
 */


/*
 * In the absense of any better format to store our ABI data in we use
 * the Read structure. Hence this module should be considered part of the
 * Read libary.
 *
 * This library also requires use of the mach-io code for the endian
 * independent machine IO.
 * 
 * The ABI results file is controlled by an index found towards
 * the end --- this is pointed to by a longword found at `IndexPO'.
 * The index consists of a number of entries, each of which is
 * four character label followed by 6 long words. The first of these
 * long words holds a simple count (starting at 1) for those cases
 * where there are multiple entries with the same label. Entries should
 * be found by label (and count), rather than their index position,
 * because entries can be ommited or new ones added. This happens when
 * ABI changes the version of their software and also depending
 * on whether the data was analysed or unalaysed. We do, however,
 * make assumptions about the relative order of entries.
 * 
 * Ideally we would have a separate module which provides a number
 * of functions to extract the data we are interested in, keeping
 * the ABI format well wrapped up and out of harms way.
 * 
 * Note that we are relying on the endian-ness of the machine being
 * appropriate so we can just read long words in as integers. This
 * should be recoded to deal with running on different endians.
 */


static char fileIdentifier[] = "$Id$";

/* ---- Imports ---- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "seqIOABI.h"
#include "Read.h"
#include "abi.h"
#include "fpoint.h"    /* IMPORT: int_to_float */
#include "mach-io.h"
#include "xalloc.h"
#include "misc.h"

/* ---- Constants ---- */

#define BasesPerLine 50 /* For output formatting */

#define baseIndex(B) ((B)=='C'?0:(B)=='A'?1:(B)=='G'?2:3)

static int header_fudge = 0;

/* DATA block numbers for traces, in order of FWO_ */
static int DataCount[4] = {9, 10, 11, 12};


/* Jason Miller 5/3/05M
 * Move replace_nl from bottom to top of file
 * just to avoid forward reference under Linux gcc.
 **/
void replace_nl(char *string) {
    char *cp;

    for (cp = string; *cp; cp++) {
	if (*cp == '\n') *cp = ' ';
    }
}




int dump_labels(FILE *fp, off_t indexO) {
    off_t entryNum = -1;
    uint_4 entryLabel, entryLw1;
    uint_2 elementtype;	/* type number as defined above */
    uint_2 elementsize;	/* size of the type in bytes */
    uint_4 numelements;	/* number of elements in data */
    uint_4 datasize;	/* total data size in bytes */
    uint_4 dataoffset;	/* offset of data in file */

    do {
      char l[6];
	entryNum++;

	if (fseek(fp, header_fudge+indexO+(entryNum*IndexEntryLength), 0) != 0)
	    return 0;

	if (!be_read_int_4(fp, &entryLabel))
	    return 0;
	if (!be_read_int_4(fp, &entryLw1))
	    return 0;
	if (!be_read_int_2(fp, &elementtype))
	    return 0;
	if (!be_read_int_2(fp, &elementsize))
	    return 0;
	if (!be_read_int_4(fp, &numelements))
	    return 0;
	if (!be_read_int_4(fp, &datasize))
	    return 0;
	if (!be_read_int_4(fp, &dataoffset))
	    return 0;

	{ // THis is very ugly and indirect, but...it works SAK
	  int_4 swapped_label = LABEL((char *)&entryLabel); // swap the byte order
	  memcpy(l, &swapped_label, 4);
	  l[4] = '\0';
	}
	if (entryLabel) {
	    unsigned char c1, c2, c3, c4;

	    c1 = (entryLabel >> 24) & 0xff;
	    c2 = (entryLabel >> 16) & 0xff;
	    c3 = (entryLabel >>  8) & 0xff;
	    c4 = (entryLabel >>  0) & 0xff;

	    if (!isprint(c1))
		break;

	    fprintf(stderr, "%s %c%c%c%c %d type:%d size:%d num:%d offset:%d\n", l, c1, c2, c3, c4, entryLw1, elementtype, elementsize, numelements, datasize,dataoffset);
	    {
	      char buffer[elementsize*numelements + 1];
	      switch(elementtype){
	      case 18:
		if (-1 != getABIString(fp, indexO, entryLabel, entryLw1, buffer)) {
		  replace_nl(buffer);
		  fprintf(stderr, "%s=%s\n", l, buffer);
		}
		break;
	      case 19:
		if (-1 != getABICString(fp, indexO, entryLabel, entryLw1, buffer)) {
		  replace_nl(buffer);
		  fprintf(stderr, "%s=%s\n", l, buffer);
		}
		break;
	      default:
		break;
	      }
	    }
	}
    } while (entryLabel);

    return 0;
}

/*
 * From the ABI results file connected to `fp' whose index starts
 * at byte offset `indexO', return in `val' the `lw'th long word
 * from the `count'th entry labelled `label'.
 * The result is 0 for failure, or index offset for success.
 */
int getABIIndexEntryLW(FILE *fp, off_t indexO,
		       uint_4 label, uint_4 count, int lw,
		       uint_4 *val) {
    off_t entryNum=-1;
    int i;
    uint_4 entryLabel, entryLw1;
    
    do {
	entryNum++;

	if (fseek(fp, header_fudge+indexO+(entryNum*IndexEntryLength), 0) != 0)
	    return 0;

	if (!be_read_int_4(fp, &entryLabel))
	    return 0;

	if (!be_read_int_4(fp, &entryLw1))
	    return 0;
    } while (!(entryLabel == label && entryLw1 == count));
    
    for(i=2; i<=lw; i++)
	if (!be_read_int_4(fp, val))
	    return 0;
    
    return indexO+(entryNum*IndexEntryLength);
}


/*
 * Gets the offset of the ABI index.
 * Returns -1 for failure, 0 for success.
 */
int getABIIndexOffset(FILE *fp, uint_4 *indexO) {
    uint_4 magic;

    /*
     * Initialise header_fudge.
     *
     * This is usually zero, but maybe we've transfered a file in MacBinary
     * format in which case we'll have an extra 128 bytes to add to all
     * our fseeks.
     */
    rewind(fp);
    be_read_int_4(fp, &magic);
    header_fudge = (magic == ABI_MAGIC ? 0 : 128);


    if ((fseek(fp, header_fudge + IndexPO, 0) != 0) ||
	(!be_read_int_4(fp, indexO)))
	return -1;
    else
	return 0;
}

/*
 * Get an "ABI String". These strings are either pointed to by the index
 * offset, or held in the offset itself when the string is <= 4 characters.
 * The first byte of the string determines its length.
 * 'string' is a buffer 256 characters long.
 *
 * Returns -1 for failure, string length for success.
 */
int getABIString(FILE *fp, off_t indexO, uint_4 label, uint_4 count,
		 char *string) {
    uint_4 off;
    uint_4 len;

    if (off = getABIIndexEntryLW(fp, indexO, label, count, 4, &len)) {
	uint_1 len2;

	if (!len)
	    return 0;

	/* Determine offset */
	if (len <= 4)
	    off += 20;
	else
	    getABIIndexEntryLW(fp, indexO, label, count, 5, &off);

	/* Read length byte */
	fseek(fp, header_fudge + off, 0);
	be_read_int_1(fp, &len2);

	/* Read data (max 255 bytes) */
	fread(string, len2, 1, fp);
	string[len2] = 0;

	return len2;
    } else
	return -1;
}

/*
 * Get an "ABI CString". These strings are either pointed to by the index
 * offset, or held in the offset itself when the string is <= 4 characters.
 * 'string' is a buffer 256 characters long.
 *
 * Returns -1 for failure, string length for success.
 */
int getABICString(FILE *fp, off_t indexO, uint_4 label, uint_4 count,
		 char *string) {
    uint_4 off;
    uint_4 len;

    if (off = getABIIndexEntryLW(fp, indexO, label, count, 4, &len)) {
	uint_1 len2;

	if (!len)
	    return 0;

	/* Determine offset */
	if (len <= 4)
	    off += 20;
	else
	    getABIIndexEntryLW(fp, indexO, label, count, 5, &off);

	fseek(fp, header_fudge + off, 0);
	/* Read data (max 255 bytes) */
	fread(string, len+2, 1, fp);
	return strlen(string);
    } else
	return -1;
}

/*
 * Get an "ABI Int_1". This is raw 1-byte integer data pointed to by the
 * offset, or held in the offset itself when the data is <= 4 characters.
 *
 * If indexO is 0 then we do not search for (or indeed use) label and count,
 * but simply assume that we are already at the correct offset and read from
 * here. (NB: This negates the length <= 4 check.)
 *
 * Returns -1 for failure, length desired for success (it'll only fill out
 * up to max_data_len elements, but it gives an indication of whether there
 * was more to come).
 */
int getABIint1(FILE *fp, off_t indexO, uint_4 label, uint_4 count,
		 uint_1 *data, int max_data_len) {
    uint_4 off;
    uint_4 len, len2;

    if (indexO) {
	if (!(off = getABIIndexEntryLW(fp, indexO, label, count, 4, &len)))
	    return -1;

	if (!len)
	    return 0;

	/* Determine offset */
	if (len <= 4)
	    off += 20;
	else
	    getABIIndexEntryLW(fp, indexO, label, count, 5, &off);
    
	len2 = MIN((uint_4)max_data_len, len);

	fseek(fp, header_fudge + off, 0);
    } else {
	len = len2 = max_data_len;
    }

    fread(data, len2, 1, fp);

    return len;
}

/*
 * Get an "ABI Int_2". This is raw 2-byte integer data pointed to by the
 * offset, or held in the offset itself when the data is <= 4 characters.
 *
 * Returns -1 for failure, length desired for success (it'll only fill out
 * up to max_data_len elements, but it gives an indication of whether there
 * was more to come).
 */
int getABIint2(FILE *fp, off_t indexO, uint_4 label, uint_4 count,
		 uint_2 *data, int max_data_len) {
    int len, l2;
    int i;

    len = getABIint1(fp, indexO, label, count, (uint_1 *)data, max_data_len*2);
    if (-1 == len)
	return -1;

    len /= 2;
    //    fprintf(stderr,"* Read %d int2 values\n", len);
    l2 = MIN(len, max_data_len);
    for (i = 0; i < l2; i++) {
	data[i] = be_int2(data[i]);
    }

    return len;
}

/*
 * Get an "ABI Int_4". This is raw 4-byte integer data pointed to by the
 * offset, or held in the offset itself when the data is <= 4 characters.
 *
 * Returns -1 for failure, length desired for success (it'll only fill out
 * up to max_data_len elements, but it gives an indication of whether there
 * was more to come).
 */
int getABIint4(FILE *fp, off_t indexO, uint_4 label, uint_4 count,
		 uint_4 *data, int max_data_len) {
    int len, l2;
    int i;

    len = getABIint1(fp, indexO, label, count, (uint_1 *)data, max_data_len*4);
    if (-1 == len)
	return -1;

    len /= 4;
    l2 = MIN(len, max_data_len);
    for (i = 0; i < l2; i++) {
	data[i] = be_int4(data[i]);
    }

    return len;
}

/*
 * Change the DATA counts for fetching traces
 */
void abi_set_data_counts(int f, int w, int o, int _) {
    DataCount[0] = f;
    DataCount[1] = w;
    DataCount[2] = o;
    DataCount[3] = _;
}

/*
 * Put the DATA counts back to their defaults.
 */
void abi_reset_data_counts(void) {
    DataCount[0] = 9;
    DataCount[1] = 10;
    DataCount[2] = 11;
    DataCount[3] = 12;
}

/*
 * Read the ABI format sequence from FILE *fp into a Read structure.
 * All printing characters (as defined by ANSII C `isprint')
 * are accepted, but `N's are translated to `-'s. In this respect we
 * are adhering (more or less) to the CSET_DEFAULT uncertainty code set.
 * 
 * Returns:
 *   Read *	- Success, the Read structure read.
 *   NULLRead	- Failure.
 */
Read *fread_abi(FILE *fp) {
    Read *read = NULLRead;
    int i;
    float fspacing;		/* average base spacing */
    char colorOrder[5];         /* Content of fwo_ field */
    uint_4 numPoints, numBases;
    uint_4 numQVs; /* SAK */
    uint_4 signalO;
    int no_bases = 0;
    int sections = read_sections(0);

    uint_4 fwo_;     /* base -> lane mapping */
    uint_4 indexO;   /* File offset where the index is */
    uint_4 baseO;    /* File offset where the bases are stored */
    uint_4 quality0; /* File offset where the QVs are stored  SAK */
    uint_4 dataCO;   /* File offset where the C trace is stored */
    uint_4 dataAO;   /* File offset where the A trace is stored */
    uint_4 dataGO;   /* File offset where the G trace is stored */
    uint_4 dataTO;   /* File offset where the T trace is stored */
    uint_4 offset;   /* Generic offset */
    uint_4 offset2;  /* Generic offset */
    uint_4 offset3;  /* Generic offset */
    uint_4 offset4;  /* Generic offset */
    uint_4 tagCount; /* Count parameter for reading PBAS and PLOC tags */

    

    /* Get the index offset */
    if (-1 == getABIIndexOffset(fp, &indexO))
	goto bail_out;
    
    ///dumplabels
	  //	  dump_labels(fp, indexO);

    /* Get the number of points */
    if (!getABIIndexEntryLW(fp,(off_t)indexO,DataEntryLabel,DataCount[0],
			    3,&numPoints)){
 
     if (!getABIIndexEntryLW(fp,(off_t)indexO,DataEntryLabel,DataCount[0],3,&numPoints)){
	goto bail_out;	
      }
    }
    
    /* Get the number of bases */
    /* #### MHH -- Try PBAS,2 first, then PBAS,1 */
    no_bases = 1;
    numBases = 0;
    for (tagCount = 2; tagCount >= 1; tagCount--)
    {
    	if (getABIIndexEntryLW(fp,(off_t)indexO,BaseEntryLabel,tagCount,3,&numBases)) {
	    no_bases = 0;
	    break;
	}
    }

    
    /* Allocate the sequence */
    if (NULLRead == (read = read_allocate(numPoints, numBases)))
	goto bail_out;	
    
    //    fprintf(stderr,"* Nbases = %d  NPoints = %d\n", numBases, numPoints);

    // This is duplicated in comments, since we want to capture it
    if (!getABIIndexEntryLW(fp,(off_t)indexO,FWO_Label,1,5,&fwo_)) {
	/* Guess at CAGT */
	fwo_ = 0x43414754;
    }
    { // THis is very ugly and indirect, but...it works SAK
      int_4 swapped_fwo_ = LABEL((char *)&fwo_); // swap the byte order
      memcpy(colorOrder, &swapped_fwo_, 4);
      colorOrder[4] = '\0';
    }


    /*
     * The order of the DATA fields is determined by the field FWO_
     * Juggle around with data pointers to get it right
     */
    if (sections & READ_SAMPLES) {
	uint_4 *dataxO[4];
	
	dataxO[0] = &dataCO;
	dataxO[1] = &dataAO;
	dataxO[2] = &dataGO;
	dataxO[3] = &dataTO;
	
	/*Get the positions of the four traces */
	if (!(getABIIndexEntryLW(fp, (off_t)indexO, DataEntryLabel,
				 DataCount[0], 5,
				 dataxO[baseIndex((char)(fwo_>>24&255))]) &&
	      getABIIndexEntryLW(fp, (off_t)indexO, DataEntryLabel,
				 DataCount[1], 5,
				 dataxO[baseIndex((char)(fwo_>>16&255))]) &&
	      getABIIndexEntryLW(fp, (off_t)indexO, DataEntryLabel,
				 DataCount[2], 5,
				 dataxO[baseIndex((char)(fwo_>>8&255))]) &&
	      getABIIndexEntryLW(fp, (off_t)indexO, DataEntryLabel,
				 DataCount[3], 5,
				 dataxO[baseIndex((char)(fwo_&255))]))) {
	    goto bail_out;
	}
    }
    
    /*************************************************************
     * Read the traces and bases information
     *************************************************************/

    if (sections & READ_SAMPLES) {
	/* Read in the C trace */
	if (fseek(fp, header_fudge + (off_t)dataCO, 0) == -1) goto bail_out;
	if(getABIint2(fp, 0, 0, 0, read->traceC, read->NPoints) <= 0) goto bail_out;
	
	/* Read in the A trace */
	if (fseek(fp, header_fudge + (off_t)dataAO, 0) == -1) goto bail_out;
	if(getABIint2(fp, 0, 0, 0, read->traceA, read->NPoints) <= 0) goto bail_out;
	
	/* Read in the G trace */
	if (fseek(fp, header_fudge + (off_t)dataGO, 0) == -1) goto bail_out;
	if(getABIint2(fp, 0, 0, 0, read->traceG, read->NPoints) <= 0) goto bail_out;
	
	/* Read in the T trace */
	if (fseek(fp, header_fudge + (off_t)dataTO, 0) == -1) goto bail_out;
	if(getABIint2(fp, 0, 0, 0, read->traceT, read->NPoints) <= 0) goto bail_out;
	
	/* Compute highest trace peak */
	for (i=0; i < read->NPoints; i++) {
	    if (read->maxTraceVal < read->traceA[i])
		read->maxTraceVal = read->traceA[i];
	    if (read->maxTraceVal < read->traceC[i])
		read->maxTraceVal = read->traceC[i];
	    if (read->maxTraceVal < read->traceG[i])
		read->maxTraceVal = read->traceG[i];
	    if (read->maxTraceVal < read->traceT[i])
		read->maxTraceVal = read->traceT[i];
	}
    }
    
    if(no_bases)
      fprintf(stderr,"* no_bases: numBases = %d\n", numBases);

    if (no_bases || !(sections & READ_BASES))
	goto skip_bases;

    /* Read in the bases */
    /* #### MHH -- Changed 4th parameter to tagCount */
    if (!(getABIIndexEntryLW(fp, (off_t)indexO, BaseEntryLabel, tagCount, 5, &baseO)
	  && (fseek(fp, header_fudge + (off_t)baseO, 0) == 0) ))
	goto bail_out;

    for (i = 0; i < (read->NBases); i++) {
	int ch;
	
	if ((ch = fgetc(fp)) == EOF)
	    goto bail_out;

	/* #### MHH -- don't turn 'N' into '-' */
	/* read->base[i] = (ch == 'N') ? '-' : (char)ch; */
	read->base[i] = (char)ch;

	/* Initialize the error probabilities. This may be overridden later */
	read->prob_A[i] = 0;
	read->prob_C[i] = 0;
	read->prob_G[i] = 0;
	read->prob_T[i] = 0;
    }
    read->base[i] = 0;
    
    /* Read in the quality values */

    /* Read in the base positions */
    /* #### MHH -- Changed 4th parameter to tagCount */
    if (-1 == getABIint2(fp, indexO, BasePosEntryLabel, tagCount, read->basePos,
			 read->NBases))
	goto bail_out;

    //   GetQualityValues, if they exist
    {
      if (-1 != getABIint1(fp, indexO, PCONLabel, 1, (uint_1 *)read->prob_A, read->NBases)){
	int i;
	for (i = 0; i < read->NBases; i++){
	  switch(tolower(read->base[i])){
	  case 'a':
	    break;
	  case 'c':
	    read->prob_C[i] = read->prob_A[i];
	    read->prob_A[i] = (char)0;
	    break;
	  case 't':
	    read->prob_T[i] = read->prob_A[i];
	    read->prob_A[i] = (char)0;
	    break;
	  case 'g':
	    read->prob_G[i] = read->prob_A[i];
	    read->prob_A[i] = (char)0;
	    break;
	  case 'n':  // set them all to zero
	  default:
	    read->prob_A[i] = (char)0;
	  }
	}
      }
    }

#ifdef FIX_CORRUPT_TRACES
/* The code inside this ifdef does not really fix the problem, it just hides it, and */
/* introduces weird behavior in client programs (like chrome_server) -- MHH. */
    /*
     * Check for corrupted traces where the bases are positioned on sample
     * coordinates which do not exist. Witnessed on some MegaBACE files.
     */
    if (read->basePos[read->NBases-1] > read->NPoints) {
	int n = read->basePos[read->NBases-1]+1;
	read->traceA = (TRACE *)xrealloc(read->traceA, n * sizeof(TRACE));
	read->traceC = (TRACE *)xrealloc(read->traceC, n * sizeof(TRACE));
	read->traceG = (TRACE *)xrealloc(read->traceG, n * sizeof(TRACE));
	read->traceT = (TRACE *)xrealloc(read->traceT, n * sizeof(TRACE));

	if (read->traceA == NULL || read->traceC == NULL ||
	    read->traceC == NULL || read->traceG == NULL)
	    goto bail_out;

	for (i = read->NPoints; i < n; i++) {
	    read->traceA[i] = 0;
	    read->traceC[i] = 0;
	    read->traceG[i] = 0;
	    read->traceT[i] = 0;
	}
	read->NPoints = n;
    }
#endif

 skip_bases:
    
    
    /*************************************************************
     * Gather useful information - the comments field
     *************************************************************/
    if (sections & READ_COMMENTS) {
	char buffer[257];
	char comment[8192], line[8192];
	char commstr[256], *commstrp;
	int clen;
	int_4 spacing;
	int_4 startPoint; // SAK
	int_4 noise; // SAK
	uint_2 i2;
	uint_4 i4;
	
	*comment = '\0';

	/* The ABI comments */
	clen = getABIString(fp, indexO, CMNTLabel, 1, commstr);
	if (clen != -1) {
	    char *p;

	    commstr[clen] = 0;
	    commstrp = commstr;

	    do {
		char line[300];
		
		if (p = strchr(commstrp, '\n'))
		    *p++ = 0;
		
		sprintf(line, "COMM=%s\n", commstrp);
		strcat(comment, line);
	    } while(commstrp = p);
	}

	
	/* Get Sample Name Offset */
	if (-1 != getABIString(fp, indexO, SMPLLabel, 1, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "NAME=%s\n", buffer);
	    strcat(comment, line);
	}
	
	/* LANE */
	if (-1 != getABIint2(fp, indexO, LANELabel, 1, &i2, 1)) {
	    sprintf(line, "LANE=%d\n", i2);
	    strcat(comment, line);
	}
#if 0
	/* LsrP */
	if (-1 != getABIint4(fp, indexO, LaserpowerLabel, 1, &i4, 1)) {
	    sprintf(line, "LsrP=%d\n", i4);
	    strcat(comment, line);
	}
	/* Tmpr */
	if (-1 != getABIint4(fp, indexO, TmprLabel, 1, &i4, 1)) {
	    sprintf(line, "Tmpr=%d\n", i4);
	    strcat(comment, line);
	}
	/* EPVt */
	if (-1 != getABIint4(fp, indexO, EPVtLabel, 1, &i4, 1)) {
	    sprintf(line, "EPVt=%d\n", i4);
	    strcat(comment, line);
	}
#endif
	// Add the ColorOrder 
      sprintf(line, "FWO_=%s\n", colorOrder);
      strcat(comment, line);
      //   Add the number of chromatogram points
      sprintf(line, "NPTS=%d\n", numPoints);
      strcat(comment, line);
      //   Add the number of chromatogram points
      sprintf(line, "NBAS=%d\n", numBases);
      strcat(comment, line);
    
	/* Get Signal Strength Offset */
	if (getABIIndexEntryLW(fp, (off_t)indexO, SignalEntryLabel, 1, 5,
			       &signalO)) {
	    int_2 C,A,G,T;
	    int_2 *base[4];
	    base[0] = &C;
	    base[1] = &A;
	    base[2] = &G;
	    base[3] = &T;

	    if (fseek(fp, header_fudge + (off_t)signalO, 0) != -1 &&
		be_read_int_2(fp, (uint_2 *)
			      base[baseIndex((char)(fwo_>>24&255))]) &&
		be_read_int_2(fp, (uint_2 *)
			      base[baseIndex((char)(fwo_>>16&255))]) &&
		be_read_int_2(fp, (uint_2 *)
			      base[baseIndex((char)(fwo_>>8&255))]) &&
		be_read_int_2(fp, (uint_2 *)
			      base[baseIndex((char)(fwo_&255))])) {
		sprintf(line, "SIGN=A=%d,C=%d,G=%d,T=%d\n",
			A, C, G, T);
		strcat(comment, line);
	    }
	}

	/* Get the spacing.. it's a float but don't worry yet */
	if (-1 != getABIint4(fp, indexO, SpacingEntryLabel, 1,
			     (uint_4 *)&spacing, 1)) {
	    fspacing = int_to_float(spacing);
	    sprintf(line, "SPAC=%-6.2f\n", fspacing);
	    strcat(comment, line);
	} else {
	    if (read->NBases)
		fspacing = (float) read->NPoints / (float) read->NBases;
	    else
		fspacing = 0;
	}

	
	/* Get primer position */
	if (getABIIndexEntryLW(fp, (off_t)indexO, PPOSLabel, 1, 5,
			       (uint_4 *)&i4)) {
	    /* ppos stores in MBShort of pointer */
	    sprintf(line, "PRIM=%d\n", (i4>>16));
	    strcat(comment, line);
	}

	/* RUND/RUNT */
	if (getABIIndexEntryLW(fp, (off_t)indexO, RUNDLabel, 1, 5, &offset) &&
	    getABIIndexEntryLW(fp, (off_t)indexO, RUNDLabel, 2, 5, &offset2) &&
	    getABIIndexEntryLW(fp, (off_t)indexO, RUNTLabel, 1, 5, &offset3) &&
	    getABIIndexEntryLW(fp, (off_t)indexO, RUNTLabel, 2, 5, &offset4)) {
	    char buffer[1025];
	    char buffer_s[1025];
	    char buffer_e[1025];
	    struct tm t;
	    time_t tsec;
	    uint_4 rund_s, rund_e, runt_s, runt_e;

	    rund_s = offset;
	    rund_e = offset2;
	    runt_s = offset3;
	    runt_e = offset4;

	    sprintf(buffer, "%04d%02d%02d.%02d%02d%02d - %04d%02d%02d.%02d%02d%02d",
		    rund_s >> 16, (rund_s >> 8) & 0xff, rund_s & 0xff,
		    runt_s >> 24, (runt_s >> 16) & 0xff, (runt_s >> 8) & 0xff,
		    rund_e >> 16, (rund_e >> 8) & 0xff, rund_e & 0xff,
		    runt_e >> 24, (runt_e >> 16) & 0xff, (runt_e >> 8) & 0xff);

	    memset(&t, 0, sizeof(t));
	    t.tm_mday = rund_s & 0xff;
	    t.tm_mon = ((rund_s >> 8) & 0xff) - 1;
	    t.tm_year = (rund_s >> 16) - 1900;
	    t.tm_hour = runt_s >> 24;
	    t.tm_min = (runt_s >> 16) & 0xff;
	    t.tm_sec = (runt_s >> 8) & 0xff;
	    t.tm_isdst = -1;
	    /*
	     * Convert struct tm to time_t. We ignore the time_t value, but
	     * the conversion process will update the tm_wday element of
	     * struct tm.
	     */
	    tsec = mktime(&t);
	    strftime(buffer_s, 1024, "%a %d %b %H:%M:%S %Y", &t);

	    t.tm_mday = rund_e & 0xff;
	    t.tm_mon = ((rund_e >> 8) & 0xff) - 1;
	    t.tm_year = (rund_e >> 16) - 1900;
	    t.tm_hour = runt_e >> 24;
	    t.tm_min = (runt_e >> 16) & 0xff;
	    t.tm_sec = (runt_e >> 8) & 0xff;
	    t.tm_isdst = -1;
	    /*
	     * Convert struct tm to time_t. We ignore the time_t value, but
	     * the conversion process will update the tm_wday element of
	     * struct tm.
	     */
	    tsec = mktime(&t);
	    strftime(buffer_e, 1024, "%a %d %b %H:%M:%S %Y", &t);

	    sprintf(line, "DATE=%s to %s\nRUND=%s\n",
		    buffer_s, buffer_e, buffer);
	    strcat(comment, line);
	}


	/* Get Dye Primer Offset */
	if (-1 != getABIString(fp, indexO, PDMFLabel, 1, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "DYEP=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Get Machine Name Offset */
	if (-1 != getABIString(fp, indexO, MCHNLabel, 1, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "MCHN=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Machine model */
	if (-1 != getABICString(fp, indexO, MODLLabel, 1, buffer)) { // added C SAK
	    replace_nl(buffer);
	    sprintf(line, "MODL=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Matrix file */
	if (-1 != getABIString(fp, indexO, MTXFLabel, 1, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "MTXF=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Base calling version */
	if (-1 != getABIString(fp, indexO, SPACLabel, 2, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "BCAL=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Software versions */
	if (-1 != getABIString(fp, indexO, SVERLabel, 1, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "VER1=%s\n", buffer);
	    strcat(comment, line);
	}
	if (-1 != getABIString(fp, indexO, SVERLabel, 2, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "VER2=%s\n", buffer);
	    strcat(comment, line);
	}


	/* Get Protocol Name Offset */
	if (-1 != getABIString(fp, indexO, ProtocolNameLabel, 2, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "PRON=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Get Protocol Version Offset */
	if (-1 != getABICString(fp, indexO, ProtocolVersionLabel, 1, buffer)) {
	    replace_nl(buffer);
	    sprintf(line, "PROV=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Get Run Name Offset SAK */
	if (-1 != getABICString(fp, indexO, RunNameLabel, 1, buffer)) {
	    sprintf(line, "RUNN=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Get TUBE SAK */
	if (-1 != getABIString(fp, indexO, TubeLabel, 1, buffer)) {
	    sprintf(line, "TUBE=%s\n", buffer);
	    strcat(comment, line);
	}

	/* Get the start point.. SAK */
	/* ASTP */
	if (-1 != getABIint2(fp, indexO, ASPTLabel, 1, &i2, 1)) {
	    sprintf(line, "ASPT=%d\n", i2);
	    strcat(comment, line);
	}



	/* Get NOIS Offset SAK */
	if (getABIIndexEntryLW(fp, (off_t)indexO, NOISLabel, 1,5,
			       &signalO)) {
	    float  C,A,G,T;
	    float *base[4];
	    base[0] = &C;
	    base[1] = &A;
	    base[2] = &G;
	    base[3] = &T;

	    if (fseek(fp, header_fudge + (off_t)signalO, 0) != -1 &&
		be_read_int_4(fp, (int_4 *)
			      base[baseIndex((char)(fwo_>>24&255))]) &&
		be_read_int_4(fp, (int_4 *)
			      base[baseIndex((char)(fwo_>>16&255))]) &&
		be_read_int_4(fp, (int_4 *)
			      base[baseIndex((char)(fwo_>>8&255))]) &&
		be_read_int_4(fp, (int_4 *)
			      base[baseIndex((char)(fwo_&255))])) {
		sprintf(line, "NOIS=A=%f,C=%f,G=%f,T=%f\n",
			A, C, G, T);
		strcat(comment, line);
	    }
	}

	/* dumplicate string and set info */
	{
	    char *s = (char *)xmalloc(strlen(comment)+1);
	    strcpy(s,comment);
	    read->info = s;
	}
    }



    /*************************************************************
     * Check base positions are in order
     *************************************************************/
    if (sections & READ_SAMPLES) {
	float pos;
	int start;


#ifdef DO_FIX_POSITIONS_ORDER
	/* The code inside this ifdef was going into infinite loops under certain conditions. Code ifdefed by Mike Holmes SAK */
	for (i = 1; i < read->NBases; ) {
	    if (read->basePos[i] < read->basePos[i-1]) {
	      //		fprintf(stderr,"fread_abi(): Base positions are not in order(%d(%d) < %d(%d)) . Fixing\n",
	      //			i, read->basePos[i], i-1, read->basePos[i-1]);

		/* pass 1 - find end of region */
		start = i - 1;
		pos = (float) read->basePos[i-1] + fspacing;
		for(;i < read->NBases && (int)read->basePos[i] < pos;i++) {
		    pos += fspacing;
		}

		/* calculate average base spacing */
		if (i < read->NBases )
		    fspacing = ((float) read->basePos[i] -
				(float) read->basePos[start]) /
				    (float)(i - start);

		/* pass 2 - adjust */
		i = start + 1;
		pos = (float) read->basePos[i-1] + fspacing;
		for(;i < read->NBases && (int)read->basePos[i] < pos;i++) {
		    read->basePos[i] = (int) pos;
		    pos += fspacing;
		}

	    } else {
		i++;
	    }
	}
#endif
    }

    
    /* SUCCESS */

    read->format = TT_ABI;
    return(read);

    /* FAILURE */
 bail_out:
    if (read)
	read_deallocate(read);

    return NULLRead;
}


/*
 * Read the ABI format sequence from file 'fn' into a Read structure.
 * All printing characters (as defined by ANSII C `isprint')
 * are accepted, but `N's are translated to `-'s. In this respect we
 * are adhering (more or less) to the CSET_DEFAULT uncertainty code set.
 * 
 * Returns:
 *   Read *	- Success, the Read structure read.
 *   NULLRead	- Failure.
 */
Read *read_abi(char *fn) {
    Read *read;
    FILE *fp;

    /* Open file */
    if ((fp = fopen(fn, "rb")) == NULL)
	return NULLRead;

    read = fread_abi(fp);
    fclose(fp);

    if (read && (read->trace_name = (char *)xmalloc(strlen(fn)+1)))
	strcpy(read->trace_name, fn);

    return read;
}
    
/*
 * Write to an ABI file - unsupported.
 */
/* ARGSUSED */
int write_abi(char *fn, Read *read) {
    fprintf(stderr, "ABI write support is unavailable\n");
    return -1;
}

/*
 * Write to an ABI file - unsupported.
 */
/* ARGSUSED */
int fwrite_abi(FILE *fp, Read *read) {
    fprintf(stderr, "ABI write support is unavailable\n");
    return -1;
}

