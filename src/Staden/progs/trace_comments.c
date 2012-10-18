/**************************************************************************
 *
 * get_comments
 *
 * Copyright (C) 2003  JCVSF Joint Technology Center
 * All rights reserved.
 *
 * Fetches all comments from a set of trace files in a source directory.
 * Writes output to standard out.
 * Each field is written as:
 * <filename>.<fieldname>=<fieldvalue>
 *
 * Usage:
 *	get_comments  dir
 *
 * Options:
 *	-h	Help
 *
 * Return codes:
 *	0	Success
 *	2	Failed to read file, or usage message displayed
 **************************************************************************/

static char filenamercsid[] = "$Id$";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "Read.h"

/* Nasty Microsoft bits */
#ifdef _MSC_VER
#  define DLL_IMPORT __declspec(dllimport)
#else
#  define DLL_IMPORT
#endif

extern DLL_IMPORT char *optarg;
extern DLL_IMPORT int optind;

void usage(void) {
    puts("Usage:");
    puts("    get_comment  dir");
    puts("\nOptions:");
    puts("    -h	Help");
    puts("\nReturn codes:");
    puts("    0	Success");
    puts("    1 Failure to allocate memory");
    puts("    2	Failed to read file, or usage message displayed");
    exit(2);
}


/* GetFileList
   Creates an array of filenames from all of the files found in directory <path>
*/
int GetFileList(char ***FileList, char *path){
  struct dirent *direntp;
  char **filelist;
  char *filename;
  DIR *dp;
  int num_files = 0;
  int count = 0;
  char *str;

  dp = opendir(path);

  //  fprintf(stderr,"* GetFileLIst %s\n", path);
  if(dp){
    while((direntp = readdir(dp)) != NULL){
      if(!strcmp(direntp->d_name,".") || !strcmp(direntp->d_name,".."))
	continue;
      num_files++;
    }
    rewinddir(dp);

    (*FileList) = filelist = (char **)calloc(num_files, (size_t)(sizeof(char *)));
    if(*FileList == (char **)NULL) return(1);

    while((direntp = readdir(dp)) != NULL){
      if(!strcmp(direntp->d_name,".") || !strcmp(direntp->d_name,".."))
	continue;
      filelist[count++] = filename = (char *)malloc(FILENAME_MAX);
      if(!filename) return(1);
      strcpy(filename, direntp->d_name);
    }
  }
  //  fprintf(stderr, "returning %d files\n", num_files);

  return(num_files);
}

int main(int argc, char **argv) {
    Read *r = NULL;
    char *directory = NULL;
    char *ident, *value;
    int ident_len, value_len;
    int i, j, found;
    int *found_args = NULL;
    char **FileList = NULL;
    char trace_filename[FILENAME_MAX]="";
    int num_traces, trace_iter, files_read = 0;
    char *str;

    if(argc != 2)
      usage();

    directory = argv[1];
    
    // Get a list of all chromatogram files in the directory
    num_traces = GetFileList(&FileList, directory);
    if(num_traces == 0){
      fprintf(stderr,"* Path %d yielded 0 files...exiting\n", num_traces);
      exit(2);
    }


  /* step through all the sequences */
  for (trace_iter = 0; trace_iter < num_traces; trace_iter++){

    if(r){
      read_deallocate(r);
    }
    //Get the file name from the iterator.
    sprintf(trace_filename, "%s/%s",directory, FileList[trace_iter]);

    /* Read the file */
    read_sections(READ_COMMENTS);
    if (NULL == (r = read_reading(trace_filename, TT_ANY))) {
      continue;  // don't worry about it
    }
    files_read++;

    if (!r->info)
	return 1;
    
    for(str = strtok(r->info,"\n"); str != NULL;       str = strtok(NULL,"\n")){
      char *name;
      char *value;
      int items = 0;
      char *start,*end;

      name = str;
      value = strchr(name,'=');
      *value = '\0'; // skip over the '='
      value++;
         
      if(!strcmp(name,"RUND")){
	start = value;
	end = strstr(value," - ");
	*end = '\0';   // terminate the start string
	end += 3;      // skip over the " - "
	fprintf(stdout,"%s.RUND=start=%s,end=%s\n", FileList[trace_iter], start,end);
      }else if(! strcmp(name,"DATE")){
	start = value;
	end = strstr(value," to ");
	*end = '\0'; // terminate the start string
	end += 4;    // skip over the " to "
	fprintf(stdout,"%s.DATE=start=%s,end=%s\n", FileList[trace_iter],start,end);
      }else{
	fprintf(stdout,"%s.%s=%s\n", FileList[trace_iter],name, value);
      }

    }
    
  }
  return (files_read == 0);  // is zero, unless we read nothing.
}
