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

#include <stdio.h>
#include <string.h>
#include <math.h>

/* #include <fcntl.h> */

#include "xalloc.h"
#include "Read.h"
#include "ztr.h"
#include "compression.h"

/*
 * ---------------------------------------------------------------------------
 * Internal prototypes for forward references.
 * ---------------------------------------------------------------------------
 */
int uncompress_chunk(ztr_chunk_t *chunk);

/*
 * ---------------------------------------------------------------------------
 * Trace writing functions.
 * These consist of several encoding functions, all with the same prototype,
 * and a single fwrite_ztr function to wrap it all up.
 * ---------------------------------------------------------------------------
 */

/*
 * ztr_write_header
 *
 * Writes a ZTR file header.
 *
 * Arguments:
 * 	fp		A FILE pointer
 *	h		A pointer to the header to write
 *
 * Returns:
 *	Success:  0
 *	Failure: -1
 */
int ztr_write_header(FILE *fp, ztr_header_t *h) {
    if (1 != fwrite(h, sizeof(*h), 1, fp))
	return -1;

    return 0;
}

/*
 * ztr_write_chunk
 *
 * Writes a ZTR chunk including chunk header and data
 *
 * Arguments:
 * 	fp		A FILE pointer
 *	chunk		A pointer to the chunk to write
 *
 * Returns:
 *	Success:  0
 *	Failure: -1
 */
int ztr_write_chunk(FILE *fp, ztr_chunk_t *chunk) {
    int4 bei4;

    /*
    {
	char str[5];
	fprintf(stderr, "Write chunk %.4s %08x length %d\n",
		ZTR_BE2STR(chunk->type, str), chunk->type, chunk->dlength);
    }
    */
    

    /* type */
    bei4 = be_int4(chunk->type);
    if (1 != fwrite(&bei4, 4, 1, fp))
	return -1;

    /* metadata length */
    bei4 = be_int4(chunk->mdlength);
    if (1 != fwrite(&bei4, 4, 1, fp))
	return -1;

    /* metadata */
    if (chunk->mdlength)
	if (chunk->mdlength != fwrite(chunk->mdata, 1, chunk->mdlength, fp))
	    return -1;

    /* data length */
    bei4 = be_int4(chunk->dlength);
    if (1 != fwrite(&bei4, 4, 1, fp))
	return -1;

    /* data */
    if (chunk->dlength != fwrite(chunk->data, 1, chunk->dlength, fp))
	return -1;

    return 0;
}

/*
 * fwrite_ztr
 *
 * Writes a ZTR file held in the ztr_t structure.
 * It is assumed that all the correct lengths, magic numbers, etc in the
 * ztr_t struct have already been initialised correctly.
 *
 * FIXME: Add a 'method' argument which encodes formats? Perhaps store this
 * in the ztr struct?
 *
 * Arguments:
 *	fp		A writable FILE pointer
 *	ztr		A pointer to the ztr_t struct to write.
 *
 * Returns:
 *	Success:  0
 *	Failure: -1
 */
int fwrite_ztr(FILE *fp, ztr_t *ztr) {
    int i;

    /* Write the header record */
    if (-1 == ztr_write_header(fp, &ztr->header))
	return -1;

    /* Write the chunks */
    for (i = 0; i < ztr->nchunks; i++) {
	if (-1 == ztr_write_chunk(fp, &ztr->chunk[i]))
	    return -1;
#if 0
	{
	    int fd;
	    char fname[1024];
	    sprintf(fname, "chunk.%d", i);
	    fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, 0666);
	    write(fd, ztr->chunk[i].data, ztr->chunk[i].dlength);
	    close(fd);
	}
#endif
    }
    
    return 0;
}

/*
 * ---------------------------------------------------------------------------
 * Trace reading functions.
 * These consist of several decoding functions, all with the same prototype,
 * and a single fread_ztr function to wrap it all up.
 * ---------------------------------------------------------------------------
 */

/*
 * ztr_read_header
 *
 * Reads a ZTR file header.
 *
 * Arguments:
 * 	fp		A FILE pointer
 *	h		Where to write the header to
 *
 * Returns:
 *	Success:  0
 *	Failure: -1
 */
int ztr_read_header(FILE *fp, ztr_header_t *h) {
    if (1 != fread(h, sizeof(*h), 1, fp))
	return -1;

    return 0;
}

/*
 * ztr_read_chunk_hdr
 *
 * Reads a ZTR chunk header and metadata, but not the main data segment.
 *
 * Arguments:
 * 	fp		A FILE pointer
 *
 * Returns:
 *	Success: a chunk pointer (malloced)
 *	Failure: NULL
 */
ztr_chunk_t *ztr_read_chunk_hdr(FILE *fp) {
    int4 bei4;
    ztr_chunk_t *chunk;

    if (NULL == (chunk = (ztr_chunk_t *)xmalloc(sizeof(*chunk))))
	return NULL;

    /* type */
    if (1 != fread(&bei4, 4, 1, fp)) {
	xfree(chunk);
	return NULL;
    }
    chunk->type = be_int4(bei4);

    /* metadata length */
    if (1 != fread(&bei4, 4, 1, fp)) {
	xfree(chunk);
	return NULL;
    }
    chunk->mdlength = be_int4(bei4);

    /* metadata */
    if (chunk->mdlength) {
	if (NULL == (chunk->mdata = (char *)xmalloc(chunk->mdlength)))
	    return NULL;
	if (chunk->mdlength != fread(chunk->mdata, 1, chunk->mdlength, fp)) {
	    xfree(chunk->mdata);
	    xfree(chunk);
	    return NULL;
	}
    } else {
	chunk->mdata = NULL;
    }

    /* data length */
    if (1 != fread(&bei4, 4, 1, fp)) {
	xfree(chunk->mdata);
	xfree(chunk);
	return NULL;
    }
    chunk->dlength = be_int4(bei4);

    return chunk;
}

void ztr_process_text(ztr_t *ztr) {
    int i;
    ztr_chunk_t **text_chunks = NULL;
    int ntext_chunks = 0;
    ztr_text_t *zt = NULL;
    int nzt = 0;
    int nalloc = 0;
    
    if (ztr->text_segments)
	/* Already done */
	return;

    text_chunks = ztr_find_chunks(ztr, ZTR_TYPE_TEXT, &ntext_chunks);
    if (!text_chunks)
	return;

    for (i = 0; i < ntext_chunks; i++) {
	char *data;
	uint4 length;
	char *ident, *value;

	/* Make sure it's not compressed */
	uncompress_chunk(text_chunks[i]);

	data = text_chunks[i]->data;
	length = text_chunks[i]->dlength;

	if (!length)
	    continue;
	
	/* Skip RAW header byte */
	data++;
	length--;

	while (*(ident = data)) {
	    data += strlen(ident)+1;
	    value = data;
	    if (value)
		data += strlen(value)+1;

	    if (nzt + 1 > nalloc) {
		nalloc += 10;
		zt = (ztr_text_t *)xrealloc(zt, nalloc * sizeof(*zt));
	    }
	    zt[nzt].ident = ident;
	    zt[nzt].value = value;
	    nzt++;
	}
    }

    ztr->text_segments = zt;
    ztr->ntext_segments = nzt;

    /*
    for (i = 0; i < ztr->ntext_segments; i++) {
	fprintf(stderr, "'%s' = '%s'\n",
		ztr->text_segments[i].ident,
		ztr->text_segments[i].value);
    }
    */

    xfree(text_chunks);
}


/*
 * fread_ztr
 *
 * Reads a ZTR file from 'fp'. This checks for the correct magic number and
 * major version number, but not minor version number.
 *
 * FIXME: Add automatic uncompression?
 *
 * Arguments:
 *	fp		A readable FILE pointer
 *
 * Returns:
 *	Success: Pointer to a ztr_t structure (malloced)
 *	Failure: NULL
 */
ztr_t *fread_ztr(FILE *fp) {
    ztr_t *ztr;
    ztr_chunk_t *chunk;
    int sections = read_sections(0);
    
    /* Allocate */
    if (NULL == (ztr = new_ztr()))
	return NULL;

    /* Read the header */
    if (-1 == ztr_read_header(fp, &ztr->header))
	return NULL;

    /* Check magic number and version */
    if (memcmp(ztr->header.magic, ZTR_MAGIC, 8) != 0)
	return NULL;

    if (ztr->header.version_major != ZTR_VERSION_MAJOR)
	return NULL;

    /* Load chunks */
    while (chunk = ztr_read_chunk_hdr(fp)) {
	char str[5];

	/*
	fprintf(stderr, "Read chunk %.4s %08x length %d\n",
		ZTR_BE2STR(chunk->type, str), chunk->type, chunk->dlength);
	*/
	switch(chunk->type) {
	case ZTR_TYPE_HEADER:
	    /* End of file */
	    return ztr;

	case ZTR_TYPE_SAMP:
	case ZTR_TYPE_SMP4:
	    if (! (sections & READ_SAMPLES)) {
		fseek(fp, chunk->dlength, SEEK_CUR);
		xfree(chunk);
		continue;
	    }
	    break;

          
	case ZTR_TYPE_BASE:
	case ZTR_TYPE_BPOS:
	case ZTR_TYPE_CNF4:
	case ZTR_TYPE_CNF1:
	case ZTR_TYPE_CSID:
	    if (! (sections & READ_BASES)) {
		fseek(fp, chunk->dlength, SEEK_CUR);
		xfree(chunk);
		continue;
	    }
	    break;

	case ZTR_TYPE_TEXT:
	    if (! (sections & READ_COMMENTS)) {
		fseek(fp, chunk->dlength, SEEK_CUR);
		xfree(chunk);
		continue;
	    }
	    break;

	case ZTR_TYPE_CLIP:
	    break;

	default:
	    fprintf(stderr, "Unknown chunk type '%s': skipping\n",
		    ZTR_BE2STR(chunk->type,str));
	    fseek(fp, chunk->dlength, SEEK_CUR);
	    xfree(chunk);
	    continue;
	}

	chunk->data = (char *)xmalloc(chunk->dlength);
	if (chunk->dlength != fread(chunk->data, 1, chunk->dlength, fp))
	    return NULL;
            
	ztr->nchunks++;
	ztr->chunk = (ztr_chunk_t *)xrealloc(ztr->chunk, ztr->nchunks *
					     sizeof(ztr_chunk_t));
	memcpy(&ztr->chunk[ztr->nchunks-1], chunk, sizeof(*chunk));
	xfree(chunk);
    }

    return ztr;
}

/*
 * ---------------------------------------------------------------------------
 * Other utility functions
 * ---------------------------------------------------------------------------
 */
/*
 * new_ztr
 *
 * Allocates and initialises a ztr_t structure
 *
 * Returns:
 *	ztr_t pointer on success
 *	NULL on failure
 */
ztr_t *new_ztr(void) {
    ztr_t *ztr;
    /* Allocate */
    if (NULL == (ztr = (ztr_t *)xmalloc(sizeof(*ztr))))
	return NULL;

    ztr->chunk = NULL;
    ztr->nchunks = 0;
    ztr->text_segments = NULL;
    ztr->ntext_segments = 0;
    ztr->delta_level = 3;

    return ztr;
}

void delete_ztr(ztr_t *ztr) {
    int i;

    if (!ztr)
	return;

    if (ztr->chunk) {
	for (i = 0; i < ztr->nchunks; i++) {
	    if (ztr->chunk[i].data)
		xfree(ztr->chunk[i].data);
	}
	xfree(ztr->chunk);
    }

    if (ztr->text_segments)
	xfree(ztr->text_segments);

    xfree(ztr);
}

/*
 * ztr_find_chunks
 *
 * Searches for chunks of a specific type.
 *
 * Returns:
 *	Array of ztr_chunk_t pointers (into the ztr struct). This is
 *	  allocated by malloc and it is the callers duty to free this.
 *	NULL if none found.
 */
ztr_chunk_t **ztr_find_chunks(ztr_t *ztr, uint4 type, int *nchunks_p) {
    ztr_chunk_t **chunks = NULL;
    int nchunks = 0;
    int i;

    for (i = 0; i < ztr->nchunks; i++) {
	if (ztr->chunk[i].type == type) {
	    chunks = (ztr_chunk_t **)xrealloc(chunks, (nchunks + 1) *
					      sizeof(*chunks));
	    chunks[nchunks++] = &ztr->chunk[i];
	}
    }
    *nchunks_p = nchunks;
    return chunks;
}

/*
 * Compresses an individual chunk using a specific format. The format is one
 * of the 'format' fields listed in the spec; one of the ZTR_FORM_ macros.
 */
int compress_chunk(ztr_chunk_t *chunk, int format, int option) {
    char *new_data = NULL;
    int new_len;

    switch (format) {
    case ZTR_FORM_RAW:
	return 0;

    case ZTR_FORM_RLE:
	new_data = rle(chunk->data, chunk->dlength, option, &new_len);
	break;

    case ZTR_FORM_ZLIB:
	new_data = zlib_huff(chunk->data, chunk->dlength, option, &new_len);
	break;

    case ZTR_FORM_DELTA1:
	new_data = decorrelate1(chunk->data, chunk->dlength, option, &new_len);
	break;

    case ZTR_FORM_DDELTA1:
	new_data = decorrelate1dyn(chunk->data, chunk->dlength, &new_len);
	break;

    case ZTR_FORM_DELTA2:
	new_data = decorrelate2(chunk->data, chunk->dlength, option, &new_len);
	break;

    case ZTR_FORM_DDELTA2:
	new_data = decorrelate2dyn(chunk->data, chunk->dlength, &new_len);
	break;

    case ZTR_FORM_DELTA4:
	new_data = decorrelate4(chunk->data, chunk->dlength, option, &new_len);
	break;

    case ZTR_FORM_16TO8:
	new_data = shrink_16to8(chunk->data, chunk->dlength, option, &new_len);
	break;

    case ZTR_FORM_32TO8:
	new_data = shrink_32to8(chunk->data, chunk->dlength, option, &new_len);
	break;

    case ZTR_FORM_FOLLOW1:
	new_data = follow1(chunk->data, chunk->dlength, option, &new_len);
	break;

    case ZTR_FORM_ICHEB:
	new_data = ichebcomp(chunk->data, chunk->dlength, &new_len);
	break;
    }

    if (!new_data) {
	fprintf(stderr, "!!ERROR!!\n");
	return -1;
    }

    //    fprintf(stderr, "Format %d => %d to %d\n", format, chunk->dlength, new_len);

    chunk->dlength = new_len;
    xfree(chunk->data);
    chunk->data = new_data;

    return 0;
}

/*
 * Uncompresses an individual chunk from all levels of compression.
 */
int uncompress_chunk(ztr_chunk_t *chunk) {
    char *new_data = NULL;
    int new_len;

    while (chunk->dlength > 0 && chunk->data[0] != ZTR_FORM_RAW) {
	switch (chunk->data[0]) {
	case ZTR_FORM_RLE:
	    new_data = unrle(chunk->data, chunk->dlength, &new_len);
	    break;

	case ZTR_FORM_ZLIB:
	    new_data = zlib_dehuff(chunk->data, chunk->dlength, &new_len);
	    break;

	case ZTR_FORM_DELTA1:
	    new_data = recorrelate1(chunk->data, chunk->dlength, &new_len);
	    break;

	case ZTR_FORM_DELTA2:
	    new_data = recorrelate2(chunk->data, chunk->dlength, &new_len);
	    break;

	case ZTR_FORM_DELTA4:
	    new_data = recorrelate4(chunk->data, chunk->dlength, &new_len);
	    break;

	case ZTR_FORM_16TO8:
	    new_data = expand_8to16(chunk->data, chunk->dlength, &new_len);
	    break;

	case ZTR_FORM_32TO8:
	    new_data = expand_8to32(chunk->data, chunk->dlength, &new_len);
	    break;

	case ZTR_FORM_FOLLOW1:
	    new_data = unfollow1(chunk->data, chunk->dlength, &new_len);
	    break;

	case ZTR_FORM_ICHEB:
	    new_data = ichebuncomp(chunk->data, chunk->dlength, &new_len);
	    break;

	default:
	    return -1;
	}
	    
	if (!new_data)
	    return -1;

	/*
	fprintf(stderr, "format %d => %d to %d\n",
		chunk->data[0], chunk->dlength, new_len);
	*/
	chunk->dlength = new_len;
	xfree(chunk->data);
	chunk->data = new_data;
    }

    return 0;
}

#if 0
/*
 * Shannon showed that for storage in base 'b' with alphabet symbols 'a' having
 * a probability of ocurring in any context of 'Pa' we should encode
 * symbol 'a' to have a storage width of -logb(Pa).
 *
 * Eg. b = 26, P(e) = .22. => width .4647277.
 *
 * We use this to calculate the entropy of a signal by summing over all letters
 * in the signal. In this case, our storage has base 256.
 */
#define EBASE 256
double entropy(unsigned char *data, int len) {
    double E[EBASE];
    double P[EBASE];
    double e;
    int i;
    
    for (i = 0; i < EBASE; i++)
        P[i] = 0;

    for (i = 0; i < len; i++)
        P[data[i]]++;

    for (i = 0; i < EBASE; i++) {
        if (P[i]) {
            P[i] /= len;
            E[i] = -(log(P[i])/log(EBASE));
        } else {
            E[i] = 0;
        }
    }

    for (e = i = 0; i < len; i++)
        e += E[data[i]];

    return e;
}
#endif

/*
 * Compresses a ztr (in memory).
 * Level is 0, 1, 2 or 3 (no compression, delta, delta + zlib,
 * chebyshev + zlib).
 */
int compress_ztr(ztr_t *ztr, int level) {
    int i;

    if (0 == level)
	return 0;

    for (i = 0; i < ztr->nchunks; i++) {

      /*
	{
	    char str[5];
	    fprintf(stderr, "---- %.4s ---- level %d from %d to ",
		    ZTR_BE2STR(ztr->chunk[i].type,str),level);
	}
      */

	switch(ztr->chunk[i].type) {
	case ZTR_TYPE_SAMP:
	case ZTR_TYPE_SMP4:
	    if (level <= 2) {
		/*
		 * Experiments show that typically a double delta does
		 * better than a single delta for 8-bit data, and the other
		 * way around for 16-bit data
		 */
		compress_chunk(&ztr->chunk[i], ZTR_FORM_DELTA2,
			       ztr->delta_level);
	    } else {
		compress_chunk(&ztr->chunk[i], ZTR_FORM_ICHEB,  0);
	    }
	    
	    compress_chunk(&ztr->chunk[i], ZTR_FORM_16TO8,  0);
	    if (level > 1) {
		compress_chunk(&ztr->chunk[i], ZTR_FORM_FOLLOW1,0);
		/*
		compress_chunk(&ztr->chunk[i],
			       ZTR_FORM_ZLIB, Z_HUFFMAN_ONLY);
		*/
		compress_chunk(&ztr->chunk[i], ZTR_FORM_RLE,  150);
		compress_chunk(&ztr->chunk[i],
			       ZTR_FORM_ZLIB, Z_HUFFMAN_ONLY);
	    }

	    break;

	case ZTR_TYPE_BASE:
	    if (level > 1) {
		compress_chunk(&ztr->chunk[i],
			       ZTR_FORM_ZLIB, Z_HUFFMAN_ONLY);
	    }
	    break;

	case ZTR_TYPE_CNF1:
	case ZTR_TYPE_CNF4:
	case ZTR_TYPE_CSID:
	    compress_chunk(&ztr->chunk[i], ZTR_FORM_DELTA1, 1);
	    compress_chunk(&ztr->chunk[i], ZTR_FORM_RLE,  77);
	    if (level > 1) {
		compress_chunk(&ztr->chunk[i],
			       ZTR_FORM_ZLIB, Z_HUFFMAN_ONLY);
	    }
	    break;

	case ZTR_TYPE_BPOS:
	    compress_chunk(&ztr->chunk[i], ZTR_FORM_DELTA4, 1);
	    compress_chunk(&ztr->chunk[i], ZTR_FORM_32TO8,  0);
	    if (level > 1) {
		compress_chunk(&ztr->chunk[i],
			       ZTR_FORM_ZLIB, Z_HUFFMAN_ONLY);
	    }
	    break;

	case ZTR_TYPE_TEXT:
	    if (level > 1) {
		compress_chunk(&ztr->chunk[i],
			       ZTR_FORM_ZLIB, Z_HUFFMAN_ONLY);
	    }
	    break;
	}

	/*
	  fprintf(stderr, "Comp length %d\n", ztr->chunk[i].dlength);
	*/
    }

    return 0;
}

/*
 * Uncompresses a ztr (in memory).
 */
int uncompress_ztr(ztr_t *ztr) {
    int i;

    for (i = 0; i < ztr->nchunks; i++) {
	/*
	{
            char str[5];
	    fprintf(stderr, "---- %.4s ----\n",
		    ZTR_BE2STR(ztr->chunk[i].type,str));
	}
	*/
	uncompress_chunk(&ztr->chunk[i]);
    }

    return 0;
}
