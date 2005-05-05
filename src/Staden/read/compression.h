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

#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include "os.h"
#include "zlib.h"

/*
 * zlib_huff()
 *
 * Compresses data using huffman encoding, as implemented by zlib.
 *
 * Arguments:
 *	uncomp		Uncompressed input data
 *	uncomp_len	Length of uncomp data
 *	comp_len	Output: length of compressed data
 *
 * Returns:
 *	Compressed data if successful
 *	NULL if not successful
 */
char *zlib_huff(char *uncomp, int uncomp_len, int strategy, int *comp_len);

/*
 * zlib_dehuff()
 *
 * Uncompresses data using huffman encoding, as implemented by zlib.
 *
 * Arguments:
 *	comp		Compressed input data
 *	comp_len	Length of comp data
 *	uncomp_len	Output: length of uncompressed data
 *
 * Returns:
 *	Uncompressed data if successful
 *	NULL if not successful
 */
char *zlib_dehuff(char *comp, int comp_len, int *uncomp_len);

/*
 * Run length encoding.
 *
 * Any run of 3 or more identical characters (up to 255 in a row) are replaced
 * by "255" followed by the number of characters followed by the character
 * value itself.
 * Any single 255 values are escaped using 255 255.
 *
 * Arguments:
 *	uncomp		Input data
 *	uncomp_len	Length of input data 'uncomp'
 *	guard		Guard byte - used to encode "N" copies of data
 *	comp_len	Output: length of compressed data
 *
 * Returns:
 *	Compressed data if successful
 *	NULL if not successful
 */
char *rle(char *uncomp, int uncomp_len, int guard, int *comp_len);

/*
 * Reverses run length encoding.
 *
 * Arguments:
 *	comp		Compressed input data
 *	comp_len	Length of comp data
 *	uncomp_len	Output: length of uncompressed data
 *
 * Returns:
 *	Uncompressed data if successful
 *	NULL if not successful
 */
char *unrle(char *comp, int comp_len, int *uncomp_len);

/*
 * decorrelate1()
 *
 * Produce successive deltas from a 1-byte array.
 *
 * Arguments:
 *	uncomp		Uncompressed data
 *	uncomp_len	Length of uncompressed data
 *	level		Differencing level (must be 1, 2 or 3)
 *	comp_len	Return: where to store new compressed length
 *
 * Returns:
 *	Success: A decorrelated buffer (malloced)
 *	Failure: NULL
 */
char *decorrelate1(char *uncomp, int uncomp_len, int level, int *comp_len);
char *decorrelate1dyn(char *s_uncomp, int uncomp_len, int *comp_len);

/*
 * recorrelate1()
 *
 * The reverse of decorrelate1()
 *
 * Arguments:
 *	comp		Compressed input data
 *	comp_len	Length of comp data
 *	uncomp_len	Output: length of uncompressed data
 *
 * Returns:
 *	Success: uncompressed data
 *	Failure: NULL
 */
char *recorrelate1(char *comp, int comp_len, int *uncomp_len);

/*
 * decorrelate2()
 *
 * Produce successive deltas from a 2-byte array (big endian)
 *
 * Arguments:
 *	uncomp		Uncompressed data
 *	uncomp_len	Length of uncompressed data
 *	level		Differencing level (must be 1, 2 or 3)
 *	comp_len	Return: where to store new compressed length
 *
 * Returns:
 *	Success: A decorrelated buffer (malloced)
 *	Failure: NULL
 */
char *decorrelate2(char *uncomp, int uncomp_len, int level, int *comp_len);
char *decorrelate2dyn(char *s_uncomp, int uncomp_len, int *comp_len);

/*
 * recorrelate2()
 *
 * The reverse of decorrelate2()
 *
 * Arguments:
 *	comp		Compressed input data
 *	comp_len	Length of comp data
 *	uncomp_len	Output: length of uncompressed data
 *
 * Returns:
 *	Success: uncompressed data
 *	Failure: NULL
 */
char *recorrelate2(char *comp, int comp_len, int *uncomp_len);

/*
 * decorrelate4()
 *
 * Produce successive deltas from a 4-byte array (big endian)
 *
 * Arguments:
 *	uncomp		Uncompressed data
 *	uncomp_len	Length of uncompressed data
 *	level		Differencing level (must be 1, 2 or 3)
 *	comp_len	Return: where to store new compressed length
 *
 * Returns:
 *	Success: A decorrelated buffer (malloced)
 *	Failure: NULL
 */
char *decorrelate4(char *uncomp, int uncomp_len, int level, int *comp_len);

/*
 * recorrelate4()
 *
 * The reverse of decorrelate4()
 *
 * Arguments:
 *	comp		Compressed input data
 *	comp_len	Length of comp data
 *	uncomp_len	Output: length of uncompressed data
 *
 * Returns:
 *	Success: uncompressed data
 *	Failure: NULL
 */
char *recorrelate4(char *comp, int comp_len, int *uncomp_len);

/*
 * shrink_16to8()
 *
 * Stores an array of 16-bit (big endian) array elements in an 8-bit array.
 * We assume that most 16-bit elements encode numbers that fit in an 8-bit
 * value. When not possible, we store a marker followed by the 16-bit value
 * stored as multiple 8-bit values.
 *
 *	uncomp		Uncompressed data
 *	uncomp_len	Length of uncompressed data (in bytes)
 *	comp_len	Return: where to store new compressed length
 *	
 * Returns:
 *	Success: An 8-bit array (malloced)
 *	Failure: NULL
 */
char *shrink_16to8(char *uncomp, int uncomp_len, int level, int *comp_len);

/*
 * expand_8to16()
 *
 * The opposite of the shrink_16to8() function.
 *
 *	comp		Compressed input data
 *	comp_len	Length of comp data (in bytes)
 *	uncomp_len	Output: length of uncompressed data (in bytes)
 *	
 * Returns:
 *	Success: Uncompressed data (char *)
 *	Failure: NULL
 */
char *expand_8to16(char *comp, int comp_len, int *uncomp_len);

/*
 * shrink_32to8()
 *
 * Stores an array of 32-bit (big endian) array elements in an 8-bit array.
 * We assume that most 32-bit elements encode numbers that fit in an 8-bit
 * value. When not possible, we store a marker followed by the 32-bit value
 * stored as multiple 8-bit values.
 *
 *	uncomp		Uncompressed data
 *	uncomp_len	Length of uncompressed data (in bytes)
 *	comp_len	Return: where to store new compressed length
 *	
 * Returns:
 *	Success: An 8-bit array (malloced)
 *	Failure: NULL
 */
char *shrink_32to8(char *uncomp, int uncomp_len, int level, int *comp_len);

/*
 * expand_8to32()
 *
 * The opposite of the shrink_32to8() function.
 *
 *	comp		Compressed input data
 *	comp_len	Length of comp data (in bytes)
 *	uncomp_len	Output: length of uncompressed data (in bytes)
 *	
 * Returns:
 *	Success: Uncompressed data (char *)
 *	Failure: NULL
 */
char *expand_8to32(char *comp, int comp_len, int *uncomp_len);

char *follow1(char *s_uncomp,
	      int uncomp_len,
	      int level,
	      int *comp_len);

char *unfollow1(char *s_comp,
		int comp_len,
		int *uncomp_len);

char *ichebcomp(char *uncomp,
		int uncomp_len,
		int *data_len);

char *ichebuncomp(char *comp,
		  int comp_len,
		  int *uncomp_len);

#endif /* _COMPRESSION_H_ */
