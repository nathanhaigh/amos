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
 * Handles compression and decompression.
 * Two functions are available. One compresses files, and the other opens
 * (read only) a compressed file and returns a FILE pointer.
 * Neither of these two are likely to work under Windows or MacOS.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "os.h" /* for ftruncate() under WINNT */

#include "compress.h"

/* SGIs do not define tempnam unless we also define _XOPEN4 && _NO_ANSIMODE */
extern char	*tempnam(const char *, const char *);

#ifdef USE_CORBA
#include "stcorba.h"
#endif

#ifdef HAVE_LIBZ
#include "zlib.h"
#endif

#ifndef MAC
#define BS 8192

/*
 * This contains the last used compression method.
 */
static int compression_used = 0;

typedef struct {
    unsigned char magic[3];
    long int offset;
    char *compress;
    char *uncompress;
    char *extension;
} Magics;

/*
 * The list of magic numbers. The attempted order for compression is the
 * order of entries in this file.
 *
 * NB: bzip gives very good (better than gzip) results, is sometimes faster for
 * compression, but unfortunately much slower (4x?) for decompression. Most
 * people won't have it anyway.
 *
 * szip is definitely the best in compression ratios, and is faster than bzip.
 * However it's still slower than gzip. For comparable ratios, but much faster,
 * see the ztr format.
 *
 * ">progname" implies that the program outputs the compressed data to stdout
 * instead of renaming the input file to input.compressed_extension.
 */
static Magics magics[] = {
    {{'B',   'Z',    '0'},	0,	"bzip",		"bzip -d <",   ".bz"},
    {{'\037','\213', '\0'},	0,	"gzip",		"gzip -d <",   ".gz"},
    {{'\037','\235', '\0'},	0,	"compress",	"uncompress <",".Z"},
    {{'\037','\036', '\0'},	0,	"pack",		"pcat",	       ".z"},
    {{'B',   'Z',    'h'},	0,	"bzip2",	"bzip2 -d <",  ".bz2"},
    {{'S',   'Z',    '\n'},	0,	">szip",	"szip -d <",   ".sz"},
};

void set_compression_method(int method) {
    compression_used = method;
}

int get_compression_method(void) {
    return compression_used;
}

/*
 * Converts compress mode strings (eg "gzip") to numbers.
 */
int compress_str2int(char *mode) {
    if (strcmp(mode, "bzip") == 0)
	return COMP_METHOD_BZIP;
    else if (strcmp(mode, "bzip2") == 0)
	return COMP_METHOD_BZIP2;
    else if (strcmp(mode, "gzip") == 0)
	return COMP_METHOD_GZIP;
    else if (strcmp(mode, "compress") == 0)
	return COMP_METHOD_COMPRESS;
    else if (strcmp(mode, "pack") == 0)
	return COMP_METHOD_PACK;
    else if (strcmp(mode, "szip") == 0)
	return COMP_METHOD_SZIP;
    else return 0;
}

/*
 * Converts compress mode strings (eg "gzip") to numbers.
 */
char *compress_int2str(int mode) {
    switch (mode) {
    case COMP_METHOD_BZIP:      return "bzip";
    case COMP_METHOD_GZIP:      return "gzip";
    case COMP_METHOD_BZIP2:     return "bzip2";
    case COMP_METHOD_COMPRESS:  return "compress";
    case COMP_METHOD_PACK:      return "pack";
    case COMP_METHOD_SZIP:      return "szip";
    }
    return "none";
}

/*
 * Compress a file using the method set in the compression_used value
 * (set by set_compression_method and fopen_compressed).
 *
 * If compression succeeds, we rename the file back its original name.
 *
 * When compression_used is 0 no compression is done.
 */
int compress_file(char *file) {
    int ret;
    char buf[2048];
    struct stat statbuf;

    /* Do nothing unless requested */
    if (compression_used == 0)
	return 0;

#ifdef HAVE_LIBZ
    /*
     * If zlib is used then we use it to implement gzip internally, thus
     * saving starting up a separate process. This is substantially faster.
     */
    if (compression_used == 2) {
	FILE *fp;
	gzFile gzfp;
	int len;
	char data[8192];

	if (NULL == (fp = fopen(file, "rb+"))) {
	    return 1;
	}

	sprintf(buf, "%s%s", file, magics[compression_used-1].extension);
	/* fprintf(stderr, "Using Zlib to %s %s to %s\n",magics[compression_used-1].compress, file, buf); */
	if (NULL == (gzfp = gzopen(buf, "wb+"))) {
	    fprintf(stderr, "error gzopen'ing %s\n", buf);
	    fclose(fp);
	    remove(buf);
	    return 1;
	}

	while ((len = fread(data, 1, 8192, fp)) > 0) {
	    gzwrite(gzfp, data, len);
	} 

	fclose(fp);
	gzclose(gzfp);

	/*fprintf(stderr, "%s'ed %s\n", magics[compression_used-1].compress, file);*/
	/* need to unlink the original file so the compressed file is renamed */
	unlink(file);
    } else {
#endif

    /* Execute the compression program */
#ifdef _WIN32
    /*
     * 15/2/99 johnt - don't have /dev/null on windows NT,
     * can't multi redirect on Win95
     */
    if (magics[compression_used-1].compress[0] == '>')
	sprintf(buf, "%s %s > %s%s",
		magics[compression_used-1].compress+1, file,
		file, magics[compression_used-1].extension);
    else
	sprintf(buf, "%s %s >nul",
		magics[compression_used-1].compress, file);
#else
    if (magics[compression_used-1].compress[0] == '>')
	sprintf(buf, "%s %s 1>%s%s 2>/dev/null",
		magics[compression_used-1].compress+1, file,
		file, magics[compression_used-1].extension);
    else
	sprintf(buf, "%s %s 1>/dev/null 2>/dev/null",
		magics[compression_used-1].compress, file);
#endif

    if ((ret = system(buf)) != 0) {
	if (ret == -1)
	    perror(buf);
	else
	    fprintf(stderr, "%s: compression failed\n", file);
	return 1;
    }

#ifdef HAVE_LIBZ
    }
#endif

    if (magics[compression_used-1].compress[0] == '>') {
	unlink(file);
    }

    /* Rename the file back */
    if (-1 == stat(file, &statbuf) && errno == ENOENT) {
	sprintf(buf, "%s%s", file, magics[compression_used-1].extension);
	rename(buf, file);
    }

    return 0;
}

/*
 * Compress a file using the method set in the compression_used value
 * (set by set_compression_method and fopen_compressed).
 *
 * If compression succeeds, we rename the file back its original name.
 *
 * When compression_used is 0 no compression is done.
 */
int fcompress_file(FILE *fp) {
    char buf[BS];
    char *fname;
    FILE *newfp;
    int len;

    /* Do nothing unless requested */
    if (compression_used == 0)
	return 0;

    /* It's also impossible if it's stdout as we can't rewind */
    if (fp == stdout)
	return 0;

    /* Copy the file pointer to a temporary file */
    /* Use tempnam() to force the use of TMP environment variable on Windows */
    if (NULL == (fname=tempnam(NULL, NULL)))
	return 0;
    if (NULL == (newfp = fopen(fname, "wb+"))){
	remove(fname);
	free(fname);
	return 0;
    }
    fflush(fp);
    rewind(fp);
    do {
	len = fread(buf, 1, BS, fp);
	if (len > 0)
	    fwrite(buf, 1, len, newfp);
    } while (!feof(fp));
    fflush(newfp);
    fclose(newfp);

    /* Compress it */
    if (compress_file(fname)) {
	remove(fname);
	free(fname);
	return 0;
    }

    /* Copy it back */
    if (NULL == (newfp = fopen(fname, "rb"))){
	remove(fname);
	free(fname);
	return 0;
    }

    rewind(fp);
    do {
	int len = fread(buf, 1, BS, newfp);
	if (len > 0)
	    fwrite(buf, 1, len, fp);
    } while (!feof(newfp));
    ftruncate(fileno(fp), ftell(fp));

    remove(fname);
    free(fname);
    fclose(newfp);
    
    return 0;
}


/*
 * Returns a file pointer of an uncompressed copy of 'file'.
 * 'file' need not exist if 'file'.ext (eg file.gz)
 * exists and can be uncompressed.
 *
 * If ofp is non NULL then the original file pointer will also be returned
 * (opened for update) to allow writing back to the original file. In cases
 * of uncompressed data this is the same as the returned file pointer.
 */
FILE *fopen_compressed(char *file, FILE **ofp) {
#ifdef USE_CORBA
    int corba = 0;
#endif
    int num_magics = sizeof(magics) / sizeof(*magics);
    int i, ret, fd, tryit, do_del = 1;
    char buf[2048], fext[1024];
    unsigned char mg[3];
    char *fname, *fptr;
    FILE *fp;

    if (NULL == (fname = tempnam(NULL, NULL)))
	return NULL;

    /*
     * Try opening the file and reading the magic number.
     * If this doesn't work, then don't worry - the filename may be
     * the original name which has been renamed due to compression.
     * (eg file.gz).
     */
/* 24/03/98 johnt - added corba support */
#ifdef USE_CORBA
    if (! strncmp(CORBATAG,file,strlen(CORBATAG)) ){
       tryit=0;
       corba = 1;
       fd=corba_open(file+strlen(CORBATAG),&fptr);

       if (fd != -1) {
	  if (3 != read(fd, mg, 3)){
	     remove(fname);
	     free(fname);
	     close(fd);
	     return NULL;
	  }
       }
       else{
	 return NULL;
       }
     } else {
#endif
       tryit = 1;
       fd = open(file, O_RDONLY);

       if (fd != -1) {
	 if (3 != read(fd, mg, 3)) {
	     close(fd);
	 } else {
	     tryit = 0;
	     fptr = file;
	 }
       }
#ifdef USE_CORBA
    }
#endif
    for (i = 0; i < num_magics; i++) {
	/* If necessary, try opening the file as 'file'.extension */
	if (tryit) {
	    sprintf(fext, "%s%s", file, magics[i].extension);
	    fptr = fext;
	    if (-1 == (fd = open(fext, O_RDONLY)))
		continue;
	    
	    if (3 != read(fd, mg, 3)) {
		close(fd);
		continue;
	    }
	}
	
	/* Check the magic numbers */
	if (mg[0] == magics[i].magic[0] && mg[1] == magics[i].magic[1] &&
	    (magics[i].magic[2] ? (mg[2] == magics[i].magic[2]) : 1)) {

#ifdef HAVE_LIBZ
	    if (i == 1) {
		int len;
		char data[8192];
		gzFile gzfp;
		/*fprintf(stderr, "Using Zlib to %s %s\n",magics[i].uncompress, fptr); */
		lseek(fd, 0, 0 /* SEEK_CURR */);
		if (NULL == (gzfp = gzdopen(fd, "rb"))) {
		    close(fd);
		    return NULL;
		}
		gzrewind(gzfp);

		if (NULL == (fp = fopen(fname, "wb+"))) {
		    remove(fname);
		    free(fname);
		    gzclose(gzfp);
		    close(fd);
		    return NULL;
		}

		while ((len = gzread(gzfp, data, 8192)) > 0) {
		    fwrite(data, 1, len, fp);
		} 

		gzclose(gzfp);
		fclose(fp);

		compression_used = i+1;
		break;
	    } else {
#endif

#ifdef _WIN32 /* 15/2/99 johnt - don't have /dev/null on windows NT, can't multi redirect on Win 95 */
	    sprintf(buf, "%s %s >%s",
		    magics[i].uncompress, fptr, fname);
#else
	    sprintf(buf, "%s %s 1>%s 2>/dev/null",
		    magics[i].uncompress, fptr, fname);
#endif
	    if ((ret = system(buf)) == 0) {
		compression_used = i+1;
		break;
	    }

#ifdef HAVE_LIBZ
	    }
#endif

	}
	
	if (tryit && fd != -1)
	    close(fd);
    }
    
    if (fd != -1) close(fd); 

    if (i == num_magics) {
	/*
	 * It's only an error if the file couldn't be found. If it
	 * exists then we'll assume that it doesn't need uncompressing.
	 */
	if (tryit) {
	    remove(fname);
	    free(fname);
	    return NULL;
	} else {
	    do_del = 0;
	    compression_used = 0;
	    remove(fname);
	    free(fname);
	    fname = fptr;
	}
    }

    /*
     * We've now got the temporary file. Open it and unlink it.
     * We can also keep the original fp open for those who need to
     * write back to it.
     */
/* 24/3/99 johnt - added corba support */
#ifdef USE_CORBA
    if( corba ){
      /* corba possibly has TWO temporary files
	 1 created by corba_open, and 1 created by the decompression
      */
      if (NULL == (fp = fopen(fname, "r+b")))
	if (NULL == (fp = fopen(fname, "rb"))){
	  remove(fname);
	  free(fname);
	  unlink(fptr); 
	  return NULL;
	}
      if (ofp) {
	if (compression_used)
	  *ofp = fopen(fptr, "r+b");
	else
	  *ofp = fp;
      }
      unlink(fptr); /* corba temp file will be held until ofp/fp closed */
    } else {
#endif
    if (NULL == (fp = fopen(fname, "r+b"))){
	if (NULL == (fp = fopen(fname, "rb"))){
	    remove(fname);
	    free(fname);
	    return NULL;
	}
    }

    if (ofp) {
	if (compression_used)
	    *ofp = fopen(tryit ? fext : file, "r+b");
	else
	    *ofp = fp;
    }
#ifdef USE_CORBA
  }
#endif

    if (do_del) {
      remove(fname);
      free(fname);
    }
    return fp;
}

/*
 * Returns a file pointer of an uncompressed copy of 'fp'.
 *
 * If ofp is non NULL then the original file pointer will also be returned
 * (opened for update) to allow writing back to the original file. In cases
 * of uncompressed data this is the same as the returned file pointer.
 */
FILE *freopen_compressed(FILE *fp, FILE **ofp) {
    int num_magics = sizeof(magics) / sizeof(*magics);
    char *fname;
    char buf[BS];
    unsigned char mg[3];
    FILE *newfp;
    int i;

    /* Test that it's compressed with 1 byte of magic (can't ungetc more) */
    mg[0] = fgetc(fp);
    ungetc(mg[0], fp);
    for (i = 0; i < num_magics; i++) {
	if (mg[0] == magics[i].magic[0]) {
	    break;
	}
    }
    if (i == num_magics) {
	return fp;
    }

    fname=tempnam(NULL, NULL);

    /* Copy the file to newfp */
    newfp = fopen(fname, "wb+");
    if (NULL == newfp){
	remove(fname);
	free(fname);
	return fp;
    }

    do {
	int len = fread(buf, 1, BS, fp);
	if (len > 0)
	    fwrite(buf, 1, len, newfp);
    } while (!feof(fp));
    fflush(newfp);
    rewind(newfp);
    rewind(fp);
    
    /* Test that it's compressed with full magic number */
    fread(mg, 1, 2, newfp);
    rewind(newfp);
    for (i = 0; i < num_magics; i++) {
	if (mg[0] == magics[i].magic[0] && mg[1] == magics[i].magic[1]) {
	    break;
	}
    }
    if (i == num_magics) {
	/* Uncompressed, just return fp */
	remove(fname);
	free(fname);
	fclose(newfp);
	return fp;
    }

#ifdef NOTDEF
    /* It's compressed, so we use fopen_compressed on fname */
    fp = fopen_compressed(fname, ofp);
    remove(fname);
    if (newfp != NULL) {
	return fp;
    } else {
	return newfp;
    }
#else
/* 24/03/99 johnt - After much consideration - this is what we think the
   above code is supposed to do ! */
   fclose(newfp);
   fp = fopen_compressed(fname,ofp);
   remove(fname);	/* ofp will hold this open if we still need it */
   free(fname);
   return fp;
#endif
}

#else /* MAC */

/* Dummy routines for MAC where compression isn't supported */

void set_compression_method(int method) {}
int get_compression_method(void) { return 0; }
int compress_file(char *file) { return 0; }
int fcompress_file(FILE *fp) { return 0; }

FILE *fopen_compressed(char *file, FILE **ofp) {
    FILE *fp;

    if (NULL == (fp = fopen(file, "rb+")))
	if (NULL == (fp = fopen(file, "rb")))
	    return NULL;

    if (ofp)
	*ofp = fp;

    return fp;
}

FILE *freopen_compressed(FILE *fp, FILE **ofp) {
    if (ofp)
	*ofp = fp;
    
    return fp;
}

#endif /* MAC */
