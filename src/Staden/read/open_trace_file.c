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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>

#ifndef PATH_MAX
#  define PATH_MAX 1024
#endif

#include "open_trace_file.h"
#include "misc.h"
#include "tar_format.h"
#include "compress.h"

static char fileIdentifier[] = "$Id$";
/*
 * Supported compression extensions. See the magics array in compress.c for
 * the full structure.
 *
 * #### MHH, Oct. 1, 2003: Added .ztr, and changed order (most likely extensions first).
 $Log$
 Revision 1.3  2005/07/23 02:07:06  mschatz
 Add scf to list of known extensions

 Revision 1.2  2005/07/23 01:31:18  mschatz
 Fix trace bug, size bug

 Revision 1.1  2005/05/05 16:44:18  aphillip
 added Staden trace IO package

 Revision 1.4  2004/02/20 22:24:31  mholmes
 Fixes bug found by Saul Kravitz (failed to open a .ab1 file when full filename speccified).

 Revision 1.4  2004/02/10 20:31:41  skravitz
 02/09/04
 Saul,

 Here's the modified file for the Staden I/O library.  I changed the find_file_dir function in open_trace_file.c.  It will now find the file if it matches the filename passed in, or if it matches the filename followed by an underbar and additional arbitrary characters.

 For example, if this is the specified path and filename:

 /local/chromo/Chromatograms/GMX/ABISSed/BMY/BMYL/BMYLU/BMYLU04TN

 it will find a file in that directory matching one of these patterns:

 BMYLU04TN
 BMYLU04TN.<extension>
 BMYLU04TN_<whatever>
 BMYLU04TN_<whatever>.<extension>

 where "whatever" can be anything, and "extension" is one of the recognized extensions (.gz, .ztr, etc.).

 I tested this using some renamed files with chrome_server (Cloe) and autoEditor, and also with the extract_seq program of the Staden package.

 Let me know if you have any questions.


 Mike

 Revision 1.3  2003/10/02 16:01:36  skravitz
 Id tag

 Revision 1.2  2003/10/02 16:01:00  skravitz
 Committed Mike Holmes' changes so that ztr extensions are searched

 */
static char *magics[] = {"", ".ztr", ".gz", ".bz", ".Z", ".z", ".bz2", ".sz", ".scf", ".ab1"};

/*
 * Initially produce a new search path where all "::"s are replaced with
 * a single ":". This is because on windows we need to include colons in
 * the search path, but colon is also our component separator.
 *
 * We explicitly add a "./" to the start of the search path
 *
 * Returns: A new search path with items separated by nul chars. Two nul
 *          chars in a row represent the end of the tokenised path.
 * Returns NULL for a failure.
 *
 * The returned data has been malloced. It is up to the caller to free this
 * memory.
 */
static char *tokenise_search_path(char *searchpath) {
    char *newsearch;
    unsigned int i, j;
    size_t len;

    if (!searchpath)
	searchpath="";

    newsearch = (char *)malloc((len = strlen(searchpath))+5);
    if (!newsearch)
	return NULL;

    newsearch[0] = '.';
    newsearch[1] = '/';
    newsearch[2] = '\0';

    for (i = 0, j = 3; i < len; i++) {
	if (i < len-1 && searchpath[i] == ':' && searchpath[i+1] == ':') {
	    newsearch[j++] = ':';
	    i++;
	    continue;
	}

	if (searchpath[i] == ':') {
	    /* Skip blank path components */
	    if (j && newsearch[j-1] != 0)
		newsearch[j++] = 0;
	} else {
	    newsearch[j++] = searchpath[i];
	}
    }

    newsearch[j++] = 0;
    newsearch[j++] = 0;
    
    return newsearch;
}

/*
 * Searches for file in the tar pointed to by tarname. If it finds it, it
 * copies it out and returns a file pointer to the temporary file,
 * otherwise we return NULL.
 *
 * If 'tarname'.index exists we will use this as a fast lookup method,
 * otherwise we just do a sequential search through the tar.
 *
 * Offset specifies a starting search position. Set this to zero if you want
 * to search through the entire tar file, otherwise set it to the byte offset
 * into the file of the tar header block for the desired file to extract.
 * (Note that the tar index file overrides this value.)
 *
 * Returns FILE pointer if found
 *         NULL if not.
 */
FILE *find_file_tar(char *file, char *tarname, size_t offset) {
    int num_magics = sizeof(magics) / sizeof(*magics);
    char path[PATH_MAX+101];
    FILE *fp;
    tar_block blk;
    int size;
    int name_len = strlen(file);

    /* Maximum name length for a tar file */
    if (name_len > 100)
	return NULL;

    /* Search the index file */
    sprintf(path, "%s.index", tarname);
    if (file_exists(path)) {
	FILE *fpind = fopen(path, "r");
	char *cp;
	int tmp_off;
	int found = 0;
	
	if (fpind) {
	    while (fgets(path, PATH_MAX+100, fpind)) {
		if (cp = strchr(path, '\n'))
		    *cp = 0;
		tmp_off = strtol(path, &cp, 10);
		while (isspace(*cp))
		    cp++;
		if (strncmp(cp, file, name_len) == 0) {
		    int i;
		    for (i = 0; i < num_magics; i++) {
			if (strcmp(&cp[name_len], magics[i]) == 0) {
			    offset = tmp_off;
			    found = 1;
			    break;
			}
		    }
		    if (found)
			break;
		}
	    }
	    fclose(fpind);

	    /* Not in index */
	    if (!found)
		return NULL;
	}
    }

    if (NULL == (fp = fopen(tarname, "rb")))
	return NULL;

    /*
     * Search through the tar file (starting from index position) looking
     * for our filename. If there was no index then we start from position 0.
     */
    fseek(fp, offset, SEEK_SET);
    while(fread(&blk, sizeof(blk), 1, fp) == 1) {
	if (!blk.header.name[0])
	    break;

	/* start with the same name... */
	if (strncmp(blk.header.name, file, name_len) == 0) {
	    int len;
	    char data[8192];
	    FILE *fpout;
	    char *fname;
	    int i;

	    /* ... but does it end with a known compression extension? */
	    for (i = 0; i < num_magics; i++) {
		if (strcmp(&blk.header.name[name_len], magics[i]) == 0) {
		    break;
		}
	    }
	    /* ... apparently not? continue then */
	    if (i == num_magics)
		continue;

	    /* Found it - copy out the data to a temporary file */
	    fname = tempnam(NULL, NULL);
	    if (NULL == (fpout = fopen(fname, "wb+"))) {
		remove(fname);
		free(fname);
		fclose(fp);
		return NULL;
	    }
	    remove(fname);
	    free(fname);

	    size = strtol(blk.header.size, NULL, 8);
	    while ((len = fread(data, 1, size > 8192 ? 8192 : size, fp)) > 0) {
		fwrite(data, 1, len, fpout);
		size -= len;
	    } 
	    
	    fclose(fp);
	    fseek(fpout, 0, SEEK_SET);
	    return fpout;
	}

	size = strtol(blk.header.size, NULL, 8);
	fseek(fp, TBLOCK*((size+TBLOCK-1)/TBLOCK), SEEK_CUR);
    }

    fclose(fp);
    return NULL;
}

/*
 * Searches for file in the directory 'dirname'. If it finds it, it opens
 * it. This also searches for compressed versions of the file in dirname
 * too.
 *
 * This function will find and open files of these forms:
 *
 *    FILENAME<suffix>
 *    FILENAME_<whatever><suffix>
 *
 * Where FILENAME is the value of the file parameter passed in.  The ability
 * to open a file whose name is extended by an underbar followed by arbitrary
 * text is a TIGR requirement.
 * 
 * The list of suffixes is specified in the global magics array.  This
 * list includes the empty string (i.e. no suffix), as well as various
 * suffixes beginning with ".", such as ".gz".
 *
 * Returns FILE pointer if found
 *         NULL if not
 *
 * #### MHH, Feb. 3, 2004: This function rewritten to support the new file
 * naming convention FILNAME_<whatever><suffix> adopted by the JTC.
 *
 */
static FILE *find_file_dir(char *file, char *dirname) {
    char path[PATH_MAX+1];
    char fullpath[PATH_MAX + 1];
    size_t len;
    int num_magics = sizeof(magics) / sizeof(*magics);
    int i;
    struct dirent *dp;
    DIR *dfd;
    char fname[MAXNAMLEN + 1], filename[MAXNAMLEN + 1], *sptr;

    /* combine dirname and file parameters into a single full file path */
    /* (Note: the file parameter itself may contain path information too) */
    len = strlen(dirname);
    if (len == 0 || strcmp(dirname, "./") == 0)
	strcpy(fullpath, file);
    else {
	if (dirname[len - 1] == '/')
	    sprintf(fullpath, "%s%s", dirname, file);
	else
	    sprintf(fullpath, "%s/%s", dirname, file);
    }

    /* see if the file exists under that exact name (with no additional extension) */
    if (file_exists(fullpath)) {
	/* yes, try to open it, and return the resulting file pointer */
        return fopen_compressed(fullpath, NULL);
    }
	
    /* now separate the full path into path and filename components */
    sptr = strrchr(fullpath, '/');
    if (sptr != NULL) {
	/* get the path information */
	len = (size_t)(sptr - fullpath);
	strncpy(path, fullpath, len);
	path[len] = '\0';

	/* special case for "./" */
	if (!strcmp(path, "."))
	    strcpy(path, "./");

	/* filename is everything following the final '/' */
	strcpy(filename, sptr + 1);
    }
    else {
	/* there is no path information */
	strcpy(path, "./");
	strcpy(filename, fullpath);
    }

    /* save length of file name that must be matched */
    len = strlen(filename);

    /* open the directory */
    if ((dfd = opendir(path)) == NULL) {
	/* error -- can't open directory path */
	return NULL;
    }

    /* read directory entries until we find a match */
    strcpy(fname, "");
    while (strlen(fname) == 0 && (dp = readdir(dfd)) != NULL) {
	/* does name from directory match the caller's name in the first len characters? */
	if ((strncmp(filename, dp->d_name, len)) == 0) {
	    /* yes -- file name must be an exact match, or followed by "_" or "." */
	    if (strlen(dp->d_name) == len) {
		/* exact match -- point to string terminator for search of magics array */
		sptr = dp->d_name + len;
	    }
	    else if (dp->d_name[len] == '_' || dp->d_name[len] == '.') {
		/* find the last "." (if any) in full file name after the required match */
		sptr = strrchr(dp->d_name + len, '.');
		if (sptr == NULL) {
		    /* no ".", point to string terminator */
		    sptr = dp->d_name + strlen(dp->d_name);
		}
	    }
	    else {
		/* this file is not a candidate */
		continue;
	    }

        /* see if the extension matches something we recognize */
        for (i = 0; i < num_magics; i++) {
            if (!strcmp(sptr, magics[i])) {
                /* found a matching extension */
                strcpy(fname, dp->d_name);
                break;
	    }
	}
    }
	}

    /* close the directory */
    closedir(dfd);

    /* did we find a matching file? */
    if (strlen(fname) > 0) {
	/* yes, build the complete path */
	if (!strcmp(path, "./"))
	    strcpy(fullpath, fname);
	else
	    sprintf(fullpath, "%s/%s", path, fname);

        fprintf(stderr, "open_trace_file fullpath: %s\n", fullpath);
	
	/* open the file and return its file pointer */
	return fopen_compressed(fullpath, NULL);
    }

    /* no match found */
    return NULL;
}

/*
 * ------------------------------------------------------------------------
 * Public functions below.
 */

/*
 * Opens a trace file named 'file'. This is initially looked for as a
 * pathname relative to a file named "relative_to". This may (for
 * example) be the name of an experiment file referencing the trace
 * file. In this case by passing relative_to as the experiment file
 * filename the trace file will be picked up in the same directory as
 * the experiment file. Relative_to may be supplied as NULL.
 *
 * 'file' is looked for at relative_to, then the current directory, and then
 * all of the locations listed in RAWDATA (which is a colon separated list).
 *
 * Returns a FILE pointer when found.
 *           NULL otherwise.
 */
FILE *open_trace_file(char *file, char *relative_to) {
    char *newsearch;
    char *ele;
    FILE *fp;

    /* Look in the same location as the incoming 'relative_to' filename */
    if (relative_to) {
	char *cp;
	char relative_path[PATH_MAX+1];
	strcpy(relative_path, relative_to);
	if (cp = strrchr(relative_path, '/'))
	    *cp = 0;
	if (fp = find_file_dir(file, relative_path))
	    return fp;
    }

    /* Not found it yet? use RAWDATA then */
    if (NULL == (newsearch = tokenise_search_path(getenv("RAWDATA"))))
	return NULL;
    
    /*
     * Step through the search path testing out each component.
     * We now look through each path element treating some prefixes as
     * special, otherwise we treat the element as a directory.
     */
    for (ele = newsearch; *ele; ele += strlen(ele)+1) {
	if (0 == strncmp(ele, "TAR=", 4)) {
	    if (fp = find_file_tar(file, ele+4, 0)) {
		free(newsearch);
		return fp;
	    }
	} else {
	    if (fp = find_file_dir(file, ele)) {
		free(newsearch);
		return fp;
	    }
	}
    }

    free(newsearch);

    return NULL;
}

int find_trace_file(void) {
    puts("Called find_trace_file");
    return 0;
}
