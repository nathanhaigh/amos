/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

/*! @file
 *  @brief newmem Implementation
 *  A slight modification of Paul Hardy's code from alnseq
 */

#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Slice.h"

//! Error Checking malloc
/*! Returns char * to a new block of memory, or prints an error if it couldnt
 *  malloc the memory.\n\n
 *
 *  @param number Number of blocks to allocate
 *  @param size Size of each block to allocate
 *  @return Block of new memory number*size bytes big
 *  @exception Exits(-1) If it can't malloc memory
 */
void * libSlice_newmem(int number, int size)
{
  void *memp;
  
  memp=malloc(number*size);
  if (memp==NULL)
  { 
    fprintf(stderr,"%s line %d: Memory Full! %d requested.\n",
            __FILE__,__LINE__, number*size);
    exit(-1);
  }

  return(memp);
}


//! Error checking malloc with explicit filename and linenumber
/*! 
 *  @param number Number of blocks to allocate
 *  @param size Size of each block to allocate
 *  @param file Filename of allocating call
 *  @param line Line number of allocating call
 *  @return Block of new memory number*size bytes big
 *  @exception Exits(-1) If it can't malloc memory
 */

void * libSlice_newmem2(int number, int size, char *file, int line)
{
  void *memp;
  
  memp=malloc(number*size);
  if (memp==NULL)
  { 
    fprintf(stderr,"%s line %d: Memory Full! %d requested.\n",
            file,line, number*size);
    exit(-1);
  }

  return(memp);
}
