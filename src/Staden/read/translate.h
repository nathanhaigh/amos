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

#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_

#include "scf.h"
#include "Read.h"
#include "expFileIO.h"

/*
 * Translates an Scf structure into a Read structure.
 * The Scf structure is left unchanged.
 *
 * Returns:
 *    A pointer to an allocated Read structure upon success.
 *    NULLRead upon failure.
 */
Read *scf2read(Scf *scf);

/*
 * Translates a Read structure into a Scf structure.
 * The Read structure is left unchanged.
 *
 * Returns:
 *    A pointer to an allocated Scf structure upon success.
 *    NULL upon failure.
 */
Scf *read2scf(Read *read);

/*
 * Translates a Read structure and an Experiment file.
 * The Read structure is left unchanged.
 *
 * Returns:
 *    A pointer to an allocated Exp_info structure upon success.
 *    NULL upon failure (FIXME: need to free memory here)
 */
Exp_info *read2exp(Read *read, char *EN);

/*
 * Controls the use of the SQ and ON lines when loading an experiment file.
 * The default (value&1 == 1) is to load these into the Read structure.
 * With value&1 == 0 we load the sequence directly from the trace file
 * (LT line).
 * value&2 controls whether to use the SL/SR fields when setting the cutoff.
 * value&2 == 0 implies to do so, and value&2 == 2 implies to not.
 *
 * Returns:
 *    The old value.
 */
int read_experiment_redirect(int value);

/*
 * Given an original positions array, update the 'basePos' and confidence
 * arrays to reflect the new sample numbers (effectively the "X coordinates").
 * The arrays to update (basePos, prob_[ACGT]) are assumed to have already
 * been allocated to the size r->NBases. The opos array is assumed to be
 * of size 'slen'.
 *
 * Specifying any of the arrays to be updated as NULL simply ignores
 * any updates required.
 * 
 * The updates are computed using 'r' and 'opos' ('slen' bases) and written
 * to the update arrays. It's possible to send over the real arrays
 * (r->basePos).
 */
void read_update_opos(Read *read, int_2 *opos, int slen, uint_2 *basePos,
		      char *prob_A, char *prob_C, char *prob_G, char *prob_T);

/*
 * Translates an experiment file to a Read structure.
 * The Exp_Info structure is left unchanged.
 *
 * Returns:
 *    A pointer to an allocated Read structure upon success.
 *    NULLRead upon failure.
 */
Read *exp2read(Exp_info *e, char *fn);

#endif
