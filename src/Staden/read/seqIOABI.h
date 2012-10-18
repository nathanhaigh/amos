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

#ifndef _seqIOABI_h_
#define _seqIOABI_h_

#include <sys/types.h> /* off_t */
#include "os.h"

/*
 * The ABI magic number - "ABIF"
 */
#define ABI_MAGIC	((int_4) ((((('A'<<8)+'B')<<8)+'I')<<8)+'F')

/*
 * The index is located towards the end of the ABI trace file.
 * It's location is given by a longword at a fixed place.
 */
#define IndexPO ((off_t)26)

#define IndexEntryLength 28

/*
 * Here are some labels we will be looking for, four chars packed
 * into an int_4
 */
#define LABEL(a) ((int_4) ((((((a)[0]<<8)+(a)[1])<<8)+(a)[2])<<8)+(a)[3])
#define DataEntryLabel    LABEL("DATA")
#define BaseEntryLabel    LABEL("PBAS")
#define BasePosEntryLabel LABEL("PLOC")
#define SpacingEntryLabel LABEL("SPAC")
#define SignalEntryLabel  LABEL("S/N%")
#define FWO_Label         LABEL("FWO_")
#define MCHNLabel         LABEL("MCHN")
#define PDMFLabel         LABEL("PDMF")
#define SMPLLabel         LABEL("SMPL")
#define PPOSLabel         LABEL("PPOS")
#define CMNTLabel         LABEL("CMNT")
#define GelNameLabel      LABEL("GELN")
#define LANELabel         LABEL("LANE")
#define RUNDLabel         LABEL("RUND")
#define RUNTLabel         LABEL("RUNT")
#define MTXFLabel         LABEL("MTXF")
#define SPACLabel         LABEL("SPAC")
#define SVERLabel         LABEL("SVER")
#define MODLLabel         LABEL("MODL")
/*
 *  Added by SAK to support JTC
 *
 */
/* Start Point */
#define ASPTLabel         LABEL("ASPt")
/* Start Point */
#define NOISLabel         LABEL("NOIS")
/* Run Name */
#define RunNameLabel      LABEL("RunN")
/* Protocol Name */
#define ProtocolNameLabel    LABEL("APFN")
/* Protocol Version */
#define ProtocolVersionLabel  LABEL("APXV")
/* Quality/Confidence Values */
#define PCONLabel LABEL("PCON")
/* TUBE (Well)  */
#define TubeLabel LABEL("TUBE")
/* Laser Power (LsrP)  */
#define LaserpowerLabel LABEL("LsrP")
/* ElectroPhersis Voltage (EPVt)  */
#define EPVtLabel LABEL("EPVt")
/* Temperature (Tmpr)  */
#define TmprLabel LABEL("Tmpr")

 /* Record #s in support of TIGR/JTC
    These are different than the standard Staden values of 1.
    According to Mike HOlmes, this should return the same values from the raw sequence files, but historical TIGR data 
    stored trace tuner data in record #1, so this will enable backward compatiblity.
 */
#define BaseEntryRecord (2)
#define BasePosEntryRecord (2)
#define PConRecord (1)
/*
 * From the ABI results file connected to `fp' whose index starts
 * at byte offset `indexO', return in `val' the `lw'th long word
 * from the `count'th entry labelled `label'.
 * The result is 0 for failure, or index offset for success.
 */
int getABIIndexEntryLW(FILE *fp, off_t indexO,
		       uint_4 label, uint_4 count, int lw,
		       uint_4 *val);

/*
 * Gets the offset of the ABI index.
 * Returns -1 for failure, 0 for success.
 */
int getABIIndexOffset(FILE *fp, uint_4 *indexO);

/*
 * Get an "ABI String". These strings are either pointed to by the index
 * offset, or held in the offset itself when the string is <= 4 characters.
 * The first byte of the string determines its length.
 * 'string' is a buffer 256 characters long.
 *
 * Returns -1 for failure, string length for success.
 */
int getABIString(FILE *fp, off_t indexO, uint_4 label, uint_4 count,
		 char *string);

/* Same, except for C string */
int getABICString(FILE *fp, off_t indexO, uint_4 label, uint_4 count, char *string);

int dump_labels(FILE *fp, off_t indexO);

/*
 * Change the DATA counts for fetching traces
 */
void abi_set_data_counts(int f, int w, int o, int _);

/*
 * Put the DATA counts back to their defaults.
 */
void abi_reset_data_counts(void);

#endif /* _seqIOABI_h_ */
