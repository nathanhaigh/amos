#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>    // SAK
#include "Read.h"
#include "traceType.h"
#include "seqIOABI.h"

static char fileIdentifier[] = "$Id$";

struct opts {
    char *name;
    char *fofn;
    char *passed;
    char *failed;
  char *error; // SAK
    int in_format;
    int out_format;
    int scale;
    int sub_background;
    int normalise;
    int compress_mode;
    int dots;
};

/*
 * Scales trace values from 0 to scale, but only if they are larger.
 */
void rescale_trace(Read *r, int scale) {
    double s;
    int i;

    if (r->maxTraceVal <= scale)
	return;
    
    s = ((double)scale)/r->maxTraceVal;

    for (i = 0; i < r->NPoints; i++) {
	r->traceA[i] = r->traceA[i] * s + 0.5;
	r->traceC[i] = r->traceC[i] * s + 0.5;
	r->traceG[i] = r->traceG[i] * s + 0.5;
	r->traceT[i] = r->traceT[i] * s + 0.5;
    }

    r->maxTraceVal = scale;
}

#if 0
/* OLD method, treats all channels together and assumes the same baseline for
 * each
 */
/*
 * Here we just take the minimum trace value and subtract this from all others.
 * The assumption is that the signal will always be 'base line' on at least
 * one of the four channels.
 */
void subtract_background(Read *r) {
    int i, min;
    for (i = 0; i < r->NPoints; i++) {
	min = 999999;
	if (r->traceA[i] < min) min = r->traceA[i];
	if (r->traceC[i] < min) min = r->traceC[i];
	if (r->traceG[i] < min) min = r->traceG[i];
	if (r->traceT[i] < min) min = r->traceT[i];
	r->traceA[i] -= min;
	r->traceC[i] -= min;
	r->traceG[i] -= min;
	r->traceT[i] -= min;
    }
}
#endif

static void subtract_background_ch(TRACE *channel, int nchannel) {
    int i, j, bg;
    int win_len = 501, win_len2 = win_len/2;
    TRACE *copy;

    if (NULL == (copy = (TRACE *)malloc(sizeof(*copy) * nchannel)))
	return;

    if (nchannel < win_len)
	win_len = nchannel;

    /* Take lowest background over win_len and subtract it */
    for (i = 0; i < nchannel; i++) {
	/* Could optimise this considerably */
	bg = INT_MAX;
	for (j = -win_len2; j < win_len2; j++) {
	    if (i+j < 0) continue;
	    if (i+j >= nchannel) break;
		
	    if (channel[i + j] < bg)
		bg = channel[i + j];
	}
	
	copy[i] = channel[i] - bg;
    }

    memcpy(channel, copy, nchannel * sizeof(*copy));
    free(copy);
}

/*
 * Find the average background level of a trace, and subtract this from the
 * peak heights.
 */
void subtract_background(Read *r) {
    subtract_background_ch(r->traceA, r->NPoints);
    subtract_background_ch(r->traceC, r->NPoints);
    subtract_background_ch(r->traceG, r->NPoints);
    subtract_background_ch(r->traceT, r->NPoints);
}

int int_compar(const void *a, const void *b) {
    return *(const TRACE *)a - *(const TRACE *)b;
}

int find_bg(TRACE *data, int ndata) {
    int i, bg;
    TRACE *copy = (TRACE *)malloc(ndata * sizeof(TRACE));

    /* Sort the trace samples by amplitude */
    memcpy(copy, data, ndata * sizeof(TRACE));
    qsort(copy, ndata, sizeof(TRACE), int_compar);

    /* Find the first non-zero value */
    for (i = 0; i < ndata && !copy[i]; i++)
	;

    /*
     * Now take a slie 0.05 through the remainder of the array and set this
     * as our background.
     */
    bg = copy[(int)((ndata - i) * 0.05 + i)];

    free(copy);
    return bg;
}

void trace_freq(TRACE *data, int ndata) {
    int i, bg;
    bg = find_bg(data, ndata);

#define MAX(a,b) ((a)>(b)?(a):(b))
    for (i = 0; i < ndata; i++) {
	data[i] = MAX(data[i] - bg, 0);
    }
}

/*
 * Separates out the dyes using a deconvolution matrix.
 * The order of elements in the matrix is C A G T.
 * A test matrix for the 373. Taken from the BASS distribution.
 */
double matrix[5][4] = {
   0.002439782,        -0.0015053751,       0.00011857301,    2.8906948e-06,
  -0.00075353298,       0.0032971052,      -0.006198165,      0.00014828549,
   0.00020249287,      -0.0017620348,       0.010530438,     -0.0020235507,
  -0.001144423,        -4.857673e-06,      -0.0018845701,     0.00395431,
  -0.12451385,          0.368916,          -2.928292,        -3.3142638
};
void separate_dyes(Read *r, double M[][4]) {
    int i, j;

    for (i = 0; i < r->NPoints; i++) {
	int C, A, G, T;
	double sep[4];

	C = r->traceC[i];
	A = r->traceA[i];
	G = r->traceG[i];
	T = r->traceT[i];

	for (j = 0; j < 4; j++)
	  sep[j] = C*M[0][j] + A*M[1][j] + G*M[2][j] + T*M[3][j] + M[4][j];

	for (j = 0; j < 4; j++)
	    sep[j] += 10;

	/* hack!
	   sep[0] += 0.1;
	   sep[1] += -0.4;
	   sep[2] += 2.9;
	   sep[3] += 3.2;
	*/

	r->traceC[i] = sep[0] < 0 ? 0 : 1000 * sep[0];
	r->traceA[i] = sep[1] < 0 ? 0 : 1000 * sep[1];
	r->traceG[i] = sep[2] < 0 ? 0 : 1000 * sep[2];
	r->traceT[i] = sep[3] < 0 ? 0 : 1000 * sep[3];
    }
}

/*
 * Find the maximum height of traces at the called bases. Use this to clip any
 * other bases.
 */
void reset_max_called_height(Read *r) {
    int i, max = 0;

    /* Find max */
    for (i=0; i < r->NBases; i++) {
	switch(r->base[i]) {
	case 'a':
	case 'A':
	    if (r->traceA[r->basePos[i]] > max)
		max = r->traceA[r->basePos[i]];
	    break;

	case 'c':
	case 'C':
	    if (r->traceC[r->basePos[i]] > max)
		max = r->traceC[r->basePos[i]];
	    break;

	case 'g':
	case 'G':
	    if (r->traceG[r->basePos[i]] > max)
		max = r->traceG[r->basePos[i]];
	    break;

	case 't':
	case 'T':
	    if (r->traceT[r->basePos[i]] > max)
		max = r->traceT[r->basePos[i]];
	    break;
	}
    }

    /* Clip to max */
    for (i = 0; i < r->NPoints; i++) {
	if (r->traceA[i] > max)
	    r->traceA[i] = max;
	if (r->traceC[i] > max)
	    r->traceC[i] = max;
	if (r->traceG[i] > max)
	    r->traceG[i] = max;
	if (r->traceT[i] > max)
	    r->traceT[i] = max;
    }
    if (r->maxTraceVal > max)
	r->maxTraceVal = max;
}

void rescale_heights(Read *r) {
    int win_len = 1000;
    int total = 0;
    int max, max2;
    int i, j, k, l;
    double max_val = 0, rescale;
    TRACE *tx[4];
    int mtv;

    tx[0] = r->traceA;
    tx[1] = r->traceC;
    tx[2] = r->traceG;
    tx[3] = r->traceT;

    if (r->NPoints < 2*win_len + 1)
	return;

    for (k = 0; k < 2; k++) {
	max2 = win_len * r->maxTraceVal;
	
	for (i = 0; i < win_len; i++) {
	    max = 0;
	    for (l = 0; l < 4; l++)
		if (tx[l][i] > max) max = tx[l][i];
	    total += max;
	}
	
	for (j = 0; i < r->NPoints; i++, j++) {
	    max = 0;
	    for (l = 0; l < 4; l++)
		if (tx[l][j] > max) max = tx[l][j];
	    total -= max;
	    
	    max = 0;
	    for (l = 0; l < 4; l++)
		if (tx[l][i] > max) max = tx[l][i];
	    total += max;
	    
	    for (l = 0; l < 4; l++) {
		if (k == 0) {
		    if (tx[l][j] * ((double)max2 / total) > max_val)
			max_val = tx[l][j] * ((double)max2 / total);
		} else {
		    tx[l][j] *= (double)max2 / total * rescale;
		}
	    }
	}

	for (; j < r->NPoints; j++) {
	    for (l = 0; l < 4; l++) {
		if (k == 0) {
		    if (tx[l][j] * ((double)max2 / total) > max_val)
			max_val = tx[l][j] * ((double)max2 / total);
		} else {
		    tx[l][j] *= (double)max2 / total * rescale;
		}
	    }
	    
	}

	if (max_val > 65535) {
	    rescale = 65535 / max_val;
	    mtv = 65535;
	} else {
	    rescale = 1.0;
	    mtv = max_val;
	}
    }

    r->maxTraceVal = mtv;
}

int convert(FILE *infp, FILE *outfp, char *infname, char *outfname,
	    struct opts *opts) {
    Read *r;

    if (NULL == (r = fread_reading(infp, infname, opts->in_format))) {
	fprintf(stderr, "failed to read file %s\n", infname);
	return 1;
    }

    if (opts->sub_background) { 
	/*
	trace_freq(r->traceA, r->NPoints);	
	trace_freq(r->traceC, r->NPoints);	
	trace_freq(r->traceG, r->NPoints);	
	trace_freq(r->traceT, r->NPoints);	
	*/
	subtract_background(r);
	/*
	separate_dyes(r, matrix);
	trace_freq(r->traceA, r->NPoints);	
	trace_freq(r->traceC, r->NPoints);	
	trace_freq(r->traceG, r->NPoints);	
	trace_freq(r->traceT, r->NPoints);
	*/
	reset_max_called_height(r);
    }

    if (opts->normalise) {
	rescale_heights(r);
    }

    if (opts->scale) {
	rescale_trace(r, opts->scale);
    }

    if (opts->name)
	r->ident = strdup(opts->name);
    else if (0 == strcmp(outfname, "(stdout)"))
	r->ident = strdup(infname);
    else
	r->ident = strdup(outfname);

    if (opts->compress_mode != -1)
	set_compression_method(opts->compress_mode);

    if (0 != (fwrite_reading(outfp, r, opts->out_format))) {
	fprintf(stderr, "failed to write file %s\n", outfname);
	read_deallocate(r);
	return 1;
    }

    read_deallocate(r);
    return 0;
}


void usage(void) {
    puts("Usage: convert_trace [options] [informat outformat] < in > out");
    puts("Or     convert_trace [options] -fofn file_of_filenames");
    puts("\nOptions are:");
    puts("\t-in_format format         Format for input (defaults to any");
    puts("\t-out_format format        Format for output (default ztr)");
    puts("\t-fofn file_of_filenames   Get \"Input Output\" names from a fofn");
    puts("\t-passed fofn              Output fofn of passed names");  
    puts("\t-error errorfn          Redirect stderr to file stderrfn (default is stderr)");
    puts("\t-failed fofn              Output fofn of failed names");  
    puts("\t-name id                  ID line for experiment file output");
    puts("\t-subtract_background      Subtracts the trace background");
    puts("\t-normalise                Normalises peak heights");
    puts("\t-scale range              Downscales peaks to 0-range");
    puts("\t-compress mode            Compress file output (not if stdout)");
    puts("\t-abi_data counts	      ABI DATA lanes to copy: eg 9,10,11,12");
    puts("\t--                        Explicitly state end of options");
    exit(1);
}

int main(int argc, char **argv) {
    struct opts opts;

    opts.in_format = TT_ANY;
    opts.out_format = TT_ZTR;
    opts.scale = 0;
    opts.sub_background = 0;
    opts.normalise = 0;
    opts.name = NULL;
    opts.compress_mode = -1;
    opts.dots = 0;
    opts.fofn = NULL;
    opts.passed = NULL;
    opts.failed = NULL;
    opts.error = NULL;
    
    for (argc--, argv++; argc > 0; argc--, argv++) {
	if (**argv != '-')
	    break;

	if (strcmp(*argv, "-scale") == 0) {
	    opts.scale = atoi(*++argv);
	    argc--;

	} else if (strcmp(*argv, "-fofn") == 0) {
	    opts.fofn = *++argv;
	    argc--;

	} else if (strcmp(*argv, "-passed") == 0) {
	    opts.passed = *++argv;
	    argc--;

	} else if (strcmp(*argv, "-failed") == 0) {
	    opts.failed = *++argv;
	    argc--;

	} else if (strcmp(*argv, "-error") == 0) {
	    opts.error = *++argv;
	  fprintf(stderr,"* Detected error argument %s\n", opts.error);
	    argc--;

	} else if (strcmp(*argv, "-subtract_background") == 0) {
	    opts.sub_background = 1;

	} else if (strcmp(*argv, "-normalise") == 0) {
	    opts.normalise = 1;

	} else if (strcmp(*argv, "-dots") == 0) {
	    opts.dots = 1;

	} else if (strcmp(*argv, "-in_format") == 0) {
	    argv++;
	    argc--;
	    if (TT_UNK == (opts.in_format = trace_type_str2int(*argv)))
		opts.in_format = atoi(*argv);

	} else if (strcmp(*argv, "-name") == 0) {
	    opts.name = *++argv;
	    argc--;

	} else if (strcmp(*argv, "-out_format") == 0) {
	    argv++;
	    argc--;
	    if (TT_UNK == (opts.out_format = trace_type_str2int(*argv)))
		opts.out_format = atoi(*argv);

	} else if (strcasecmp(*argv, "-compress") == 0) {
	    opts.compress_mode = compress_str2int(*++argv);
	    argc--;

	} else if (strcmp(*argv, "-abi_data") == 0) {
	    int c1, c2, c3, c4;
	    argc--;
	    if (4 == sscanf(*++argv, "%d,%d,%d,%d", &c1, &c2, &c3, &c4)) {
		abi_set_data_counts(c1, c2, c3, c4);
	    } else {
		usage();
	    }

	} else if (strcmp(*argv, "--") == 0) {
	    break;

	} else {
	    usage();
	}
    }

    if (argc == 2) {
	/* Old syntax, for backwards compatibility */

	if (TT_UNK == (opts.in_format = trace_type_str2int(argv[0])))
	    opts.in_format = atoi(argv[0]);
	if (TT_UNK == (opts.out_format = trace_type_str2int(argv[1])))
	    opts.out_format = atoi(argv[1]);
    } else if (argc != 0) {
	usage();
    }


    // Added by SAK: Allow redirection of error output to file, due to problems with Java exec
    if( NULL != opts.error){
      int fd;

      fprintf(stderr,"* Redirecting stderr to %s\n", opts.error);

      close(2); // close fd with stderr
      fd = creat(opts.error, -1  );
      if(fd == -1){
	exit(-1);
      }
    }
    if (!opts.fofn) {
	return convert(stdin, stdout, "(stdin)", "(stdout)", &opts);
    } else {
	FILE *fpin, *fpout;
	FILE *fppassed = NULL, *fpfailed = NULL;
	char *infname, *outfname;
	int ret, ret_all = 0;
	char line[8192], line2[8192];

	FILE *fofn_fp;

	if (NULL == (fofn_fp = fopen(opts.fofn, "r"))) {
	    perror(opts.fofn);
	    return -1;
	}

	if (opts.passed && NULL == (fppassed = fopen(opts.passed, "w"))) {
	    perror(opts.passed);
	    return -1;
	}

	if (opts.failed && NULL == (fpfailed = fopen(opts.failed, "w"))) {
	    perror(opts.failed);
	    return -1;
	}

	while (fgets(line, 8192, fofn_fp) != NULL) {
	    int i, j, len;
	    time_t ret_time; // SAK
	    /* Find input and output name, escaping spaces as needed */
	    len = strlen(line);
	    outfname = NULL;
	    for (i = j = 0; i < len; i++) {
		if (line[i] == '\\' && i != len-1) {
		    line2[j++] = line[++i];
		} else if (line[i] == ' ') {
		    line2[j++] = 0;
		    outfname = &line2[j];
		} else if (line[i] != '\n') {
		    line2[j++] = line[i];
		}
	    }
	    line2[j] = 0;
	    infname = line2;

	    /* Open input and output files */
	    if (NULL == (fpin = fopen(infname, "rb"))) {
		char buf[2048];
		sprintf(buf, "ERROR %s", infname);
		perror(buf);
		if (opts.dots) {
		    fputc('!', stdout);
		    fflush(stdout);
		}
		if (fpfailed)
		    fprintf(fpfailed, "%s\n", infname);
		continue;
	    }

	    if (outfname) {
		if (NULL == (fpout = fopen(outfname, "wb+"))) {
		    char buf[2048];
		    sprintf(buf, "ERROR %s", outfname);
		    perror(buf);
		    fclose(fpin);
		    if (opts.dots) {
			fputc('!', stdout);
			fflush(stdout);
		    }
		    if (fpfailed)
			fprintf(fpfailed, "%s\n", infname);
		    continue;
		}
	    } else {
		outfname = "(stdout)";
		fpout = stdout;
	    }

	    /* Convert */
	    ret = convert(fpin, fpout, infname, outfname, &opts);
	    ret_time = time(NULL); // SAK
	    ret_all |= ret;
	    if (opts.dots) {
		fputc(ret ? '!' : '.', stdout);
		fflush(stdout);
	    }
	    if (ret) {
		if (fpfailed)
		  fprintf(fpfailed, "%s %s", infname, ctime(&ret_time)); // SAK
	    } else {
		if (fppassed)
		  fprintf(fppassed, "%s %s", infname, ctime(&ret_time)); // SAK
	    }

	    /* Tidy up */
	    fclose(fpin);
	    if (fpout != stdout)
		fclose(fpout);
	}

	if(ret_all)
	  fprintf(stderr,"* ret_all = %d\n", ret_all);

	fclose(fofn_fp);

	return ret_all;
    }

    return 0;
}
